from __future__ import absolute_import, division, print_function

## (need add import compute_argument_name in the __init__.py of kimpy)
#import kimpy
#print (kimpy.compute_argument_name.get_number_of_compute_arguments())

# OK
#from kimpy import compute_argument_name
#print (compute_argument_name.get_number_of_compute_arguments())

# OK
#import kimpy.compute_argument_name as a
#print (a.get_number_of_compute_arguments())

# OK
#import kimpy as kim
#import kimpy.compute_argument_name
#print (kim.compute_argument_name.get_number_of_compute_arguments())

## (need add `import compute_argument_name` or add `compute_argument_name` to
## `__all__` in __init__.py of kimpy)
#from kimpy import *
#print (compute_argument_name.get_number_of_compute_arguments())

