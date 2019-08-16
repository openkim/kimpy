"""
Library of routines for processing KIM Simulator Models
"""

import re
import os
import subprocess
import json

try:
    import requests

    requests_available = True
except ImportError:
    requests_available = False

__version__ = '1.4.0'
__author__ = 'Ellad Tadmor'


class KIMSMError(Exception):
    def __init__(self, msg):
        super(KIMSMError, self).__init__(msg)
        self.msg = msg

    def __str__(self):
        return self.msg


class KIMWebCallError(Exception):
    def __init__(self, msg):
        super(KIMWebCallError, self).__init__(msg)
        self.msg = msg

    def __str__(self):
        return self.msg


def which(program):
    '''
    Locate a program file in the user's path.  Return 'None' if the
    program is unavailable.
    '''

    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return None


def web_call(url, querydict):
    '''
    Perform a web call to the specified 'url' with the list of arguments in 'querydict'.
    Try the following methods in order: requests module, curl, wget using the first one
    that is available.
    '''
    if requests_available:
        # module requests available; use that.
        try:
            resultvec = requests.post(url, data=querydict).json()
        except:
            raise KIMWebCallError(
                'ERROR: KIM web call using module requests to url "%s" failed.' % url
            )
    else:
        # module requests NOT available, so try curl.
        curl_failed = True
        if which('curl') != None:
            # construct curl command
            curlargs = []
            for k, v in list(querydict.items()):
                curlargs.append("--data-urlencode")
                curlargs.append("%s=%s" % (k, v))
            curlcmd = ['curl'] + curlargs + [url]
            # execute curl command and capture output
            try:
                curlstub = subprocess.Popen(
                    curlcmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE
                )
                resultvecstr, curlstderr = curlstub.communicate()
                # turn string returned by curl into list
                resultvec = ast.literal_eval(resultvecstr)
                curl_failed = False
            except:
                # curl failed for some reason
                pass
        # check curl success
        if curl_failed:
            # curl is either not available or it failed, so try wget
            wget_failed = True
            if which('wget') != None:
                # construct wget command
                wgetargs = []
                for k, v in list(querydict.items()):
                    wgetargs.append(("%s=%s" % (k, v)).replace('"', '\"'))
                wgeturl = url + "?" + ('&').join(wgetargs)
                try:
                    wgetstub = subprocess.Popen(
                        ["wget", '-qO', '-', wgeturl],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                    )
                    resultvecstr, wgetstderr = wgetstub.communicate()
                    # turn string returned by curl into list
                    resultvec = ast.literal_eval(resultvecstr)
                    wget_failed = False
                except:
                    pass
            if wget_failed:
                raise KIMWebCallError(
                    'ERROR: Unable to perform KIM web call (tried requests, curl and wget).'
                )

    if len(resultvec) == 0:
        raise KIMWebCallError('ERROR: KIM query failed to return a result.')

    return resultvec[0]


def _construct_querydict(stringvar, qtype):
    '''
    Reformat the list of arguments in "stringvar" for known queries into a dictionary
    that can be passed to a web call.

    For example, given the query:

            @<kim-test-result(test="TE_849680434885", model="MO_958932894036_001", species=["Si"],
            prop="structure-cubic-crystal-npt", keys=["a"], units=["angstrom"])>@

    Returns:
            {'test': 'TE_849680434885', 'model': 'MO_958932894036_001', 'species': '["Si"]',
            'prop': 'structure-cubic-crystal-npt', 'keys': '["a"]', 'units': '["angstrom"]'}
    '''
    if qtype.lower() == "kim-test-result":
        qargs = ["test", "model", "species", "prop", "keys", "units"]
    elif qtype.lower() == "kim-reference-data":
        qargs = ["ref_data", "species", "prop", "keys", "units"]
    else:
        raise KIMWebCallError(
            'ERROR: _construct_query called with unknown query type = "%s"' % qtype
        )
    # Find the opening parenthesis
    try:
        ind_open_paren = stringvar.index("(")
    except:
        raise KIMWebCallError(
            'ERROR: Open parenthesis "(" missing in "%s" query.' % qtype
        )
    # Find arguments and put in dictionary
    querydict = {}
    for i in range(0, len(qargs)):
        # Find argument i
        try:
            ind_arg = stringvar.index(qargs[i], ind_open_paren)
        except:
            raise KIMWebCallError(
                'ERROR: Argument "%s" missing in "%s" query.' % (qargs[i], qtype)
            )
        # For argument i, find the value between the "=" after the argument and the
        # following "," or ")"
        try:
            ind_value_start = stringvar.index("=", ind_arg) + 1
        except:
            raise KIMWebCallError(
                'ERROR: Argument "%s" not followed by "=" in "%s" query.'
                % (qargs[i], qtype)
            )
        end_delimiter = re.compile(",|\)")
        match_obj = end_delimiter.search(stringvar[ind_value_start:])
        try:
            ind_value_end = match_obj.start() + ind_value_start
        except:
            raise KIMWebCallError(
                'ERROR: Value of argument "%s" not followed by "," or ")" in "%s" query.'
                % (qargs[i], qtype)
            )
        value = stringvar[ind_value_start:ind_value_end]
        if (value.startswith('"') and not value.endswith('"')) or (
            value.endswith('"') and not value.startswith('"')
        ):
            raise KIMWebCallError(
                'ERROR: Value of argument "%s" is followed by a string that only starts or ends in quotes in "%s" query.'
                % (qargs[i], qtype)
            )
        if value.startswith('"') and value.endswith('"'):
            value = value[1:-1]
        # Place arg-value pair into query dictionary
        querydict[qargs[i]] = value
    return querydict


def template_check_and_strip_space(stringraw):
    '''
    Check that KIM @<...>@ templates are correct and strip all spaces inside them
    '''
    stringvar = ''
    intemplate = False  # True when i points to character within @<...>@
    for i in range(0, len(stringraw)):
        if stringraw[i : i + 2] in ("<@", "@>"):
            raise KIMSMError(
                'ERROR: Backwards template marker, either "<@" or "@>" detected.'
            )
        if intemplate:
            if stringraw[i : i + 2] == '@<':
                raise KIMSMError('ERROR: Invalid template "@< ... @<" detected.')
            if stringraw[i : i + 2] == '>@':
                intemplate = False
            if stringraw[i : i + 1] != ' ':
                stringvar += stringraw[i : i + 1]
        else:
            if stringraw[i : i + 2] == '>@':
                raise KIMSMError(
                    'ERROR: Invalid template ">@" detected without preceding "@<".'
                )
            if stringraw[i : i + 2] == '@<':
                intemplate = True
            stringvar += stringraw[i : i + 1]
    if intemplate:
        raise KIMSMError('ERROR: Unclosed template "@<..." detected.')
    return stringvar


def _do_atom_type_template_replacements(stringvar, son, atom_type_son_list_string):
    '''
    Replace @<atom-type-*>@ templates with chemical symbols or atomic numbers as
    required:
    If son='sym', do @<atom-type-sym-*>@ template replacements.
    If son='num', do @<atom-type-num-*>@ template replacements.
    '''
    if len(atom_type_son_list_string) != 0:
        son = son.strip().lower()
        # Replace @<atom-type-sym-list>@ templates
        stringvar = stringvar.replace(
            "@<atom-type-" + son + "-list>@", atom_type_son_list_string
        )
        # Loop over all atom types and replace @<atom-type-sym-*>@ templates
        for i in range(0, len(atom_type_son_list_string.split(' '))):
            atom_type_string = (
                "@<atom-type-" + son + "-" + str(i + 1).strip() + ">@"
            )  # atom types numbered from 1
            stringvar = stringvar.replace(
                atom_type_string, atom_type_son_list_string.split(' ')[i]
            )
        # Make sure there are no atom-type-sym-n templates left in stringvar.
        # If there are it means that the requested number n is unavailable
        if re.search("@<atom-type-" + son + "-[0-9]+", stringvar):
            req = re.search("@<atom-type-" + son + "-[0-9]+", stringvar).group(0)
            raise KIMSMError(
                'ERROR: Model requests "%s" out of range of available mapping "%s" from LAMMPS script.'
                % (req[2:], atom_type_son_list_string)
            )
    return stringvar


def template_substitution(
    stringraw,
    param_filenames,
    sm_dirname,
    atom_type_sym_list_string="",
    atom_type_num_list_string="",
):
    '''
    Substitute standard KIM templates of the form @<...>@ with appropriate text

    stringraw: string
       The string containing the templates that need to be processed.

    param_filenames: list of strings
       List of the model parameter filenames for template @<parameter-file-n>@

    atom_type_sym_list_string: string (optional)
       String containing the atom type to chemical symbol mapping specified by the
       LAMMPS pair_coeff.  If this argument is not provided the @<atom-type-sym-list>@
       and @<atom-type-sym-n>@ templates are not replaced.

    atom_type_num_list_string: string (optional)
       String containing the atom type to atomic number mapping specified by the
       LAMMPS pair_coeff.  If this argument is not provided the @<atom-type-num-list>@
       and @<atom-type-num-n>@ templates are not replaced.

    Supported templates:

      @<kim-test-result(test=None, model=None, species=None, prop=None, keys=None, units=None)

         Perform test result query to openkim.org and replace with result.
         Example:
            @<kim-test-result(test="TE_849680434885", model="MO_958932894036_001", species=["Si"],
            prop="structure-cubic-crystal-npt", keys=["a"], units=["SI"])>@

      @<kim-reference-data(ref_data=None, species=None, prop=None, keys=None, units=None)

         Perform reference data query to openkim.org and replace with result.
         Example:
            @<kim-reference-data(ref_data="RD_XXXXXXXXXXXX", species=["Si"],
            prop="structure-cubic-crystal-npt", keys=["a"], units=["SI"])>@

      @<files-path>@:

         Replace with the path to the location of the simulator model files (e.g.
         parameter files).

      @<parameter-file-n>@:

         Replace with the name of parameter file number "n" of simulator model
         passed in the list "param_filenames".

      @<atom-type-sym-list>@:

         Replace with atom type to species mapping appearing on the pair_coeff line
         passed in the string "atom_type_sym_list_string".  Note: if this argument is not passed
         then "@<atom-type-sym-list>@" and @<atom-type-sym-n>@ templates are not replaced.

      @<atom-type-sym-n>@:

         Replace with the 'n'th member of 'atom-type-sym-list'. Substitution only
         made if 'atom-type-sym-list' is provided.

      @<atom-type-num-list>@:

         Replace with atom type to atomic number based on the pair_coeff line
         passed in the string "atom_type_num_list_string".  Note: if this argument is not passed
         then "@<atom-type-sym-list>@" and @<atom-type-sym-n>@ templates are not replaced.

      @<atom-type-sym-n>@:

         Replace with the 'n'th member of 'atom-type-sym-list'. Substitution only
         made if 'atom-type-sym-list' is provided.

    '''
    if len(stringraw) == 0:
        return ''
    #
    # Remove spaces within @<...>@
    #
    stringvar = template_check_and_strip_space(stringraw)
    #
    # Process @<files-path>@ template:
    #
    stringvar = stringvar.replace("@<files-path>@", sm_dirname)
    #
    # Process @<atom-type-*-list>@ and @<atom-type-*-n>@ templates
    #
    stringvar = _do_atom_type_template_replacements(
        stringvar, 'sym', atom_type_sym_list_string
    )
    stringvar = _do_atom_type_template_replacements(
        stringvar, 'num', atom_type_num_list_string
    )
    #
    # Process @<parameter-file-n>@ template
    #
    # Loop over all parameter files and replace with parameter file names
    for i in range(0, len(param_filenames)):
        param_string = "@<parameter-file-" + str(i + 1).strip() + ">@"
        stringvar = stringvar.replace(param_string, param_filenames[i])
    # Make sure there are no parameter file templates left in stringvar.
    # If there are it means that the requested number is unavailable
    if re.search("@<parameter-file-[0-9]+", stringvar):
        raise KIMSMError('ERROR: Unavailable parameter file requested.')
    #
    # Process @<kim-test-result(...)>@ template
    #
    template_list = ["kim-test-result", "kim-reference-data"]
    url_endpoint_list = ["get_test_result", "get_reference_data"]
    for i in range(0, len(template_list)):
        template_regex = "@<" + template_list[i] + "(.+?)>@"
        for m in re.finditer(template_regex, stringvar.lower()):
            # pull out template
            t_start = m.start()
            t_end = m.end()
            t_string = stringvar[t_start:t_end]
            # turn list of arguments to dictionary
            querydict = _construct_querydict(t_string, template_list[i])
            # perform query and get result
            url = "https://query.openkim.org/api/" + url_endpoint_list[i]
            result = web_call(url, querydict)
            # replace template with result
            stringvar = stringvar[:t_start] + str(result) + stringvar[t_end:]
    #
    # Report an error if there are any unprocessed templates
    #
    if re.search("@<.*>@", stringvar):
        req = re.search("@<.*>@", stringvar).group(0)
        # Found an "unknown" template, but make sure this is not
        # a "atom-type-sym-list" or "atom-type-sym-n" template, which is
        # left in intentionally if atom_type_sym_list_string is not
        # provided.
        if (
            req != "@<atom-type-sym-list>@"
            and not re.search("@<atom-type-sym-[0-9]+>@", req)
            and req != "@<atom-type-num-list>@"
            and not re.search("@<atom-type-num-[0-9]+>@", req)
        ):
            raise KIMSMError('ERROR: Encountered unknown template: "%s"' % req)
    #
    # Exit with result
    #
    return stringvar


def get_atomic_number(sym):
    '''
    Return the atomic number associated with chemical symbol "sym".
    '''
    # Define table of atomic numbers
    atomic_number = dict(
        H=1,
        He=2,
        Li=3,
        Be=4,
        B=5,
        C=6,
        N=7,
        O=8,
        F=9,
        Ne=10,
        Na=11,
        Mg=12,
        Al=13,
        Si=14,
        P=15,
        S=16,
        Cl=17,
        Ar=18,
        K=19,
        Ca=20,
        Sc=21,
        Ti=22,
        V=23,
        Cr=24,
        Mn=25,
        Fe=26,
        Co=27,
        Ni=28,
        Cu=29,
        Zn=30,
        Ga=31,
        Ge=32,
        As=33,
        Se=34,
        Br=35,
        Kr=36,
        Rb=37,
        Sr=38,
        Y=39,
        Zr=40,
        Nb=41,
        Mo=42,
        Tc=43,
        Ru=44,
        Rh=45,
        Pd=46,
        Ag=47,
        Cd=48,
        In=49,
        Sn=50,
        Sb=51,
        Te=52,
        I=53,
        Xe=54,
        Cs=55,
        Ba=56,
        La=57,
        Ce=58,
        Pr=59,
        Nd=60,
        Pm=61,
        Sm=62,
        Eu=63,
        Gd=64,
        Tb=65,
        Dy=66,
        Ho=67,
        Er=68,
        Tm=69,
        Yb=70,
        Lu=71,
        Hf=72,
        Ta=73,
        W=74,
        Re=75,
        Os=76,
        Ir=77,
        Pt=78,
        Au=79,
        Hg=80,
        Tl=81,
        Pb=82,
        Bi=83,
        Po=84,
        At=85,
        Rn=86,
        Fr=87,
        Ra=88,
        Ac=89,
        Th=90,
        Pa=91,
        U=92,
        Np=93,
        Pu=94,
        Am=95,
        Cm=96,
        Bk=97,
        Cf=98,
        Es=99,
        Fm=100,
        Md=101,
        No=102,
        Lr=103,
        Rf=104,
        Db=105,
        Sg=106,
        Bh=107,
        Hs=108,
        Mt=109,
        Ds=110,
        Rg=111,
        Cn=112,
        Nh=113,
        Fl=114,
        Mc=115,
        Lv=116,
        Ts=117,
        Og=118,
    )
    try:
        num = atomic_number[sym.strip().title()]
    except:
        raise KeyError(
            'Cannot map chemical symbol to atomic number; Unknown symbol {}.'.format(
                sym.strip().title()
            )
        )

    return num


def get_kim_api_simulator_model_utility():
    '''
    Find the location of the kim_api_simulator_model utility and return
    the executable including full path.
    '''
    try:
        libexec_path = (
            subprocess.check_output(
                ["pkg-config", "--variable=libexecdir", "libkim-api"],
                universal_newlines=True,
            )
            .strip()
            .rstrip("/")
        )
    except:
        raise KIMSMError('ERROR: Unable to obtain libexec-path from KIM API utility.')
    return os.path.join(libexec_path, "kim-api", "kim-api-simulator-model")


def is_simulator_model(model):
    '''
    Returns True if 'model' is a Simulator Model
    '''
    kim_api_sm_util = get_kim_api_simulator_model_utility()
    try:
        ret = subprocess.call([kim_api_sm_util, model], stdout=subprocess.PIPE)
        # The "stdout = ..." part of the command prevents the output
        # generated by the utility to be echoed to the screen
    except:
        raise KIMSMError(
            'ERROR: Unable to determine whether %s is a Simulator Model.' % model
        )
    if ret == 0:
        is_sm = True  # success; this is a Simulator Model
    elif ret == 1:
        raise KIMSMError('ERROR: Model %s does not appear to be installed.' % model)
    else:
        is_sm = False
    return is_sm


class ksm_object(object):
    '''
    ksm_object class which initializes a KIM Simulator Model object
    and provides access to query and process this information.
    '''

    # Required keys in KIM Simulator Model metadata
    ksm_required_keys = [
        "extended-id",
        "simulator-name",
        "simulator-version",
        "model-init",
        "model-defn",
        "supported-species",
        "units",
    ]

    # Get kim-api-simulator-model utility executable including full path
    kim_api_sm_util = get_kim_api_simulator_model_utility()

    def _create_SM_files_directory(self):
        '''
        If it does not exist, create a directory under the current directory
        named KIMID-files where "KIMID" is the short KIM ID of the SM.
        '''
        # Make subdirectory for parameter files (if it doesn't exist)
        self.sm_dirname = "./" + self.kim_id.strip() + "-files"
        if not os.path.exists(self.sm_dirname):
            try:
                os.makedirs(self.sm_dirname)
            except:
                raise KIMSMError('ERROR: Unable to create directory for parameter files.')

    def _extract_model_metadata(self):
        '''
        Obtain KIM Simulator Model metadata file from KIM API utility
        '''
        # Get kim-simulator-model metadata file
        try:
            metadata_filename = subprocess.check_output(
                [self.kim_api_sm_util, self.extended_kim_id, "metadata-file", "name"]
            ).strip()
            metadata_filename = str(metadata_filename)
        except:
            raise KIMSMError('ERROR: Unable to obtain metadata file name.')
        # construct name of simulator json file (kim-id.json)
        model_metadata_file = self.sm_dirname + "/" + metadata_filename.strip()
        # open metadata file for writing
        try:
            f = open(model_metadata_file, "w")
        except:
            raise KIMSMError(
                'ERROR: Unable to open KIM Simulator Model metadata file "%s" for writing.'
                % model_metadata_file
            )
        # call KIM API routine to get back simulator model metadata file for the
        # specified extended KIM ID and write it to "model_metadata_file" defined above
        try:
            exit_status = subprocess.call(
                [self.kim_api_sm_util, self.extended_kim_id, "metadata-file", "data"],
                stdout=f,
            )
        except OSError as e:
            if e.errno == os.errno.ENOENT:
                raise KIMSMError('ERROR: "%s" utility not found.' % self.kim_api_sm_util)
            else:
                raise KIMSMError(
                    'ERROR: Unable to write KIM Simulator Model metadata file "%s".'
                    % model_metadata_file
                )
        if exit_status != 0:
            raise KIMSMError(
                'ERROR: KIM Simulator Model "%s" not installed in KIM API.'
                % self.extended_kim_id
            )
        # close json file
        f.close()
        return model_metadata_file

    def _extract_model_paramfiles(self):
        '''
        Obtain KIM Simulator Model parameter files from KIM API utility
        '''
        # initialize list of parameter file names
        param_filenames = []
        # get number of parameter files
        try:
            number_param_files = int(
                subprocess.check_output(
                    [
                        self.kim_api_sm_util,
                        self.extended_kim_id,
                        "number-of-parameter-files",
                    ]
                )
            )
        except:
            raise KIMSMError('ERROR: Unable to obtain number of parameter files.')
        # loop of parameter files, obtain them from KIM API routine, and store in local files
        for i in range(0, number_param_files):
            # Get parameter file name
            try:
                param_filename = subprocess.check_output(
                    [self.kim_api_sm_util, self.extended_kim_id, str(1 + i), "name"]
                ).strip()
                param_filename = str(param_filename)
            except:
                raise KIMSMError(
                    'ERROR: Unable to obtain file name for parameter file %d.' % (1 + i)
                )
            parameter_file = self.sm_dirname + "/" + param_filename
            try:
                f = open(parameter_file, "w")
            except:
                raise KIMSMError(
                    'ERROR: Unable to open parameter file "%s" for writing.'
                    % parameter_file
                )
            # call KIM API routine to obtain parameter file "1+i" for the specified
            # extended KIM ID and write it to "parameter_file" defined above
            try:
                subprocess.call(
                    [self.kim_api_sm_util, self.extended_kim_id, str(1 + i), "data"],
                    stdout=f,
                )
            except:
                raise KIMSMError(
                    'ERROR: Unable to write parameter file "%s".' % parameter_file
                )
            param_filenames.append(parameter_file)
            f.close()
        return param_filenames

    def __init__(
        self, extended_kim_id='NONE', model_metadata_file='NONE', param_filenames=[]
    ):
        '''
        Creates a KIM Simulator Model object.

        Read in SM metadata and parameter files. Make sure metadata is valid JSON
        and contains all required keys.
        '''

        # Validate input
        if extended_kim_id == 'NONE' and model_metadata_file == 'NONE':
            raise KIMSMError(
                'ERROR: Cannot initialize KIM Simulator Model. Model name not provided.'
            )

        # Determine if in local mode (metadata file and parameter files provided
        # as arguments and not obtained from installed SM) or check mode (verifying
        # correctness of metadata file).
        local_or_check_mode = False
        if model_metadata_file != 'NONE':
            local_or_check_mode = True

        # Unless in local or check mode, verify that the requested model is an SM
        # and create subdirectory for SM files
        self.extended_kim_id = extended_kim_id
        if not local_or_check_mode:
            if not is_simulator_model(self.extended_kim_id):
                raise KIMSMError(
                    'ERROR: Request model %s is not a Simulator Model.'
                    % self.extended_kim_id
                )
            # Try to parse model name assuming it has an extended KIM ID format
            # to obtain short KIM_ID. This is used to name the directory
            # containing the SM files.
            try:
                self.kim_id = re.search(
                    'SM_[0-9]{12}_[0-9]{3}', self.extended_kim_id
                ).group(0)
            except AttributeError:
                self.kim_id = (
                    self.extended_kim_id
                )  # Model name does not contain a short KIM ID,
            # Create subdirectory where SM files will be placed
            self._create_SM_files_directory()

        # Get metadata filename and parameter filenames
        if not local_or_check_mode:
            self.model_metadata_file = self._extract_model_metadata()
            self.param_filenames = self._extract_model_paramfiles()
        else:
            self.model_metadata_file = model_metadata_file
            self.param_filenames = param_filenames

        # Check that metadata file exists and is valid JSON
        if os.path.isfile(self.model_metadata_file):
            with open(self.model_metadata_file) as json_data:
                try:
                    self.model_metadata_dict = json.load(json_data)
                except:
                    raise KIMSMError(
                        'ERROR: KIM Simulator Model metadata file "%s" is invalid JSON.'
                        % self.model_metadata_file
                    )
        else:
            raise KIMSMError(
                'ERROR: KIM Simulator Model metadata file "%s" not found.'
                % self.model_metadata_file
            )

        # Check that all required keys are present and any contained templates are valid
        for key in self.ksm_required_keys:
            if key not in self.model_metadata_dict:
                raise KIMSMError(
                    'ERROR: Required key "%s" not found in KIM Simulator Model metadata file "%s".'
                    % (key, self.model_metadata_file)
                )
            if isinstance(self.model_metadata_dict[key], list):
                for stringraw in self.model_metadata_dict[key]:
                    dummy = template_check_and_strip_space(stringraw)
            else:
                stringraw = self.model_metadata_dict[key]
                dummy = template_check_and_strip_space(stringraw)

    def full_strip(self, retstr):
        '''
        Remove all leading and trailing spaces, newline characters, replace
        tabs with single spaces, and reduce multiple internal spaces to one

        '''
        retstr = retstr.strip()  # remove spaces and newlines
        retstr = retstr.replace('\t', ' ')  # replace tabs by spaces
        retstr = re.sub(' +', ' ', retstr)  # replace multiple spaces with one
        return retstr

    def get_model_simulator_name(self):
        '''
        Get the KIM Simulator Model simulator name
        '''
        return self.full_strip(self.model_metadata_dict["simulator-name"])

    def get_model_simulator_version(self):
        '''
        Get the KIM Simulator Model simulator name
        '''
        return self.full_strip(self.model_metadata_dict["simulator-version"])

    def get_model_extended_kim_id(self):
        '''
        Get the KIM Simulator Model extended KIM ID
        '''
        return self.full_strip(self.model_metadata_dict["extended-id"])

    def get_model_supported_species(self):
        '''
        Get the KIM Simulator Model list of supported species.
        '''
        supported_species_string = self.model_metadata_dict["supported-species"]
        supported_species_list = supported_species_string.split(' ')
        for i in range(0, len(supported_species_list)):
            supported_species_list[i] = self.full_strip(supported_species_list[i])
        return supported_species_list

    def get_model_init_lines(self):
        '''
        Get the KIM Simulator Model initialization lines
        '''
        retlst = self.model_metadata_dict["model-init"]
        for i in range(0, len(retlst)):
            retlst[i] = self.full_strip(retlst[i])
        return retlst

    def get_model_defn_lines(self):
        '''
        Get the KIM Simulator Model definitions lines
        '''
        retlst = self.model_metadata_dict["model-defn"]
        for i in range(0, len(retlst)):
            retlst[i] = self.full_strip(retlst[i])
        return retlst

    def get_model_param_filenames(self):
        '''
        Get a list of the KIM Simulator Model parameter filenames
        '''
        return self.param_filenames

    def get_model_units(self):
        '''
        Get the KIM Simulator Model definitions lines
        '''
        return self.full_strip(self.model_metadata_dict["units"])
