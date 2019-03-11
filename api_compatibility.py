from collections import OrderedDict

# key: kimpy version
# value['target']: the version of kim-api that kimpy targets for
# value['backward']: the version of kim-api that kimpy is backward compatible with
CompatibleTable = OrderedDict()
CompatibleTable['0.1.1'] = {'target': '2.0.0', 'backward': '2.0.0'}
CompatibleTable['0.1.2'] = {'target': '2.0.0', 'backward': '2.0.0'}
CompatibleTable['0.1.3'] = {'target': '2.0.0', 'backward': '2.0.0'}
CompatibleTable['0.2.0'] = {'target': '2.0.0', 'backward': '2.0.0'}
CompatibleTable['0.2.1'] = {'target': '2.0.1', 'backward': '2.0.0'}
CompatibleTable['0.2.2'] = {'target': '2.0.1', 'backward': '2.0.0'}


def compare_version(x, y):
    """"check the version relation of x and y in the format a.b.c.

    Return:
    '<' if x smaller than y
    '=' if x equal y
    '>' is x larger than y
    """
    x_major, x_minor, x_patch = [int(i) for i in x.split('.')]
    y_major, y_minor, y_patch = [int(i) for i in y.split('.')]
    if x_major > y_major:
        return '>'
    elif x_major < y_major:
        return '<'
    else:
        if x_minor > y_minor:
            return '>'
        elif x_minor < y_minor:
            return '<'
        else:
            if x_patch > y_patch:
                return '>'
            elif x_patch < y_patch:
                return '<'
            else:
                return '='


def suggest_kimpy(api_v):
    """Suggestion a kimpy version for the given api version.

    Always suggest the latest kimpy compatible with the given api.
    """

    b = None
    t = None
    # search backward
    for key, value in CompatibleTable.items():
        if value['target'] == api_v:
            t = key
        if value['backward'] == api_v:
            b = key
    if b:
        return b
    else:
        return t


def check_kim_api_compatibility(kimpy_v, api_v):

    # check whether we have kimpy for the specified api
    latest_kimpy = next(reversed(CompatibleTable))  # last key in OrderedDict
    latest_target = CompatibleTable[latest_kimpy]['target']
    oldest_target = '2.0.0'
    r = compare_version(latest_target, api_v)
    if r == '<':
        c1 = False
    else:
        r = compare_version(oldest_target, api_v)
        if r == '>':
            c1 = False
        else:
            c1 = True

    # check whether the current kimpy is compatible with the api
    target = CompatibleTable[kimpy_v]['target']
    backward = CompatibleTable[kimpy_v]['backward']
    r = compare_version(target, api_v)
    if r == '<':
        c2 = False
    else:
        r = compare_version(backward, api_v)
        if r == '>':
            c2 = False
        else:
            c2 = True

    if not c1 or not c2:
        msg1 = ('KIM-API version "{}" detected, which is incompatible with the '
                'current kimpy of version "{}".\nTry one of the below:\n'
                .format(api_v, kimpy_v))
        msg2 = ('  (1) Update KIM-API. The current kimpy is compatible with '
                'KIM-API versions "{}" ~ "{}" (including).\n'
                .format(backward, target))
        if not c1:
            msg3 = ('  (2) Use the latest kimpy. If you are using the latest kimpy, '
                    'it seems there is not a kimpy compatible with KIM-API version "{}" '
                    'yet. Ask Mingjian Wen (wenxx151@umn.edu) to update kimpy.\n'
                    .format(api_v))
        else:
            compatible_kimpy = suggest_kimpy(api_v)
            msg3 = ('  (2) Switch kimpy version. The detected KIM-API is '
                    'compatible with kimpy version "{}".\n'.format(compatible_kimpy))
        msg = msg1 + msg2 + msg3

    else:
        msg = None
    return msg


if __name__ == '__main__':
    msg = check_kim_api_compatibility('0.1.6', '2.0.0')
    print(msg)
