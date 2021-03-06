# -- LICENSE BEGIN --
#
# Copyright (c) 2015-2018, Lawrence Livermore National Security, LLC.
#
# Produced at the Lawrence Livermore National Laboratory
#
# Written by
#   Michael Bentley (mikebentley15@gmail.com),
#   Geof Sawaya (fredricflinstone@gmail.com),
#   and Ian Briggs (ian.briggs@utah.edu)
# under the direction of
#   Ganesh Gopalakrishnan
#   and Dong H. Ahn.
#
# LLNL-CODE-743137
#
# All rights reserved.
#
# This file is part of FLiT. For details, see
#   https://pruners.github.io/flit
# Please also read
#   https://github.com/PRUNERS/FLiT/blob/master/LICENSE
#
# Redistribution and use in source and binary forms, with or
# without modification, are permitted provided that the following
# conditions are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the disclaimer below.
#
# - Redistributions in binary form must reproduce the above
#   copyright notice, this list of conditions and the disclaimer
#   (as noted below) in the documentation and/or other materials
#   provided with the distribution.
#
# - Neither the name of the LLNS/LLNL nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL
# SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
# IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#
# Additional BSD Notice
#
# 1. This notice is required to be provided under our contract
#    with the U.S. Department of Energy (DOE). This work was
#    produced at Lawrence Livermore National Laboratory under
#    Contract No. DE-AC52-07NA27344 with the DOE.
#
# 2. Neither the United States Government nor Lawrence Livermore
#    National Security, LLC nor any of their employees, makes any
#    warranty, express or implied, or assumes any liability or
#    responsibility for the accuracy, completeness, or usefulness of
#    any information, apparatus, product, or process disclosed, or
#    represents that its use would not infringe privately-owned
#    rights.
#
# 3. Also, reference herein to any specific commercial products,
#    process, or services by trade name, trademark, manufacturer or
#    otherwise does not necessarily constitute or imply its
#    endorsement, recommendation, or favoring by the United States
#    Government or Lawrence Livermore National Security, LLC. The
#    views and opinions of authors expressed herein do not
#    necessarily state or reflect those of the United States
#    Government or Lawrence Livermore National Security, LLC, and
#    shall not be used for advertising or product endorsement
#    purposes.
#
# -- LICENSE END --

'Implements the update subcommand'

import argparse
import os
import sys
import toml

import flitconfig as conf
import flitutil

brief_description = 'Updates the Makefile based on flit-config.toml'

def main(arguments, prog=sys.argv[0]):
    'Main logic here'
    parser = argparse.ArgumentParser(
        prog=prog,
        description='''
            Updates the Makefile based on flit-config.toml.  The Makefile
            is autogenerated and should not be modified manually.  If there
            are things you want to replace or add, you can use custom.mk
            which is included at the end of the Makefile.  So, you may add
            rules, add to variables, or override variables.
            ''',
        )
    parser.add_argument('-C', '--directory', default='.',
                        help='The directory to initialize')
    args = parser.parse_args(arguments)

    tomlfile = os.path.join(args.directory, 'flit-config.toml')
    try:
        projconf = toml.load(tomlfile)
    except FileNotFoundError:
        print('Error: {0} not found.  Run "flit init"'.format(tomlfile),
              file=sys.stderr)
        return 1
    flitutil.fill_defaults(projconf)

    makefile = os.path.join(args.directory, 'Makefile')
    if os.path.exists(makefile):
        print('Updating {0}'.format(makefile))
    else:
        print('Creating {0}'.format(makefile))

    host = projconf['hosts'][0]
    dev_build = host['dev_build']
    dev_compiler_name = dev_build['compiler_name']
    dev_optl = dev_build['optimization_level']
    dev_switches = dev_build['switches']
    matching_dev_compilers = [x for x in host['compilers']
                              if x['name'] == dev_compiler_name]
    assert len(matching_dev_compilers) > 0, \
            'Compiler name {0} not found'.format(dev_compiler_name)
    assert len(matching_dev_compilers) < 2, \
            'Multiple compilers with name {0} found'.format(dev_compiler_name)
    dev_compiler_bin = matching_dev_compilers[0]['binary']
    #if '/' in dev_compiler_bin:
    #    dev_compiler_bin = os.path.realpath(dev_compiler_bin)

    ground_truth = host['ground_truth']
    gt_compiler_name = ground_truth['compiler_name']
    gt_optl = ground_truth['optimization_level']
    gt_switches = ground_truth['switches']
    matching_gt_compilers = [x for x in host['compilers']
                             if x['name'] == gt_compiler_name]
    assert len(matching_dev_compilers) > 0, \
            'Compiler name {0} not found'.format(gt_compiler_name)
    assert len(matching_dev_compilers) < 2, \
            'Multiple compilers with name {0} found'.format(gt_compiler_name)
    # TODO: use the compiler mnemonic rather than the path
    gt_compiler_bin = matching_gt_compilers[0]['binary']
    #if '/' in dev_compiler_bin:
    #    gt_compiler_bin = os.path.realpath(gt_compiler_bin)

    supported_compiler_types = ('clang', 'gcc', 'intel')
    base_compilers = {x: None for x in supported_compiler_types}
    for compiler in host['compilers']:
        assert compiler['type'] in supported_compiler_types, \
            'Unsupported compiler type: {}'.format(compiler['type'])
        assert base_compilers[compiler['type']] is None, \
            'You can only specify one of each type of compiler.'
        base_compilers[compiler['type']] = compiler['binary']

    test_run_args = ''
    if not projconf['run']['timing']:
        test_run_args = '--no-timing'
    else:
        test_run_args = ' '.join([
            '--timing-loops', str(projconf['run']['timing_loops']),
            '--timing-repeats', str(projconf['run']['timing_repeats']),
            ])

    given_compilers = [key.upper() for key, val in base_compilers.items()
                       if val is not None]
    replacements = {
        'dev_compiler': dev_compiler_bin,
        'dev_optl': dev_optl,
        'dev_switches': dev_switches,
        'ground_truth_compiler': gt_compiler_bin,
        'ground_truth_optl': gt_optl,
        'ground_truth_switches': gt_switches,
        'flit_include_dir': conf.include_dir,
        'flit_lib_dir': conf.lib_dir,
        'flit_data_dir': conf.data_dir,
        'flit_script_dir': conf.script_dir,
        'flit_version': conf.version,
        'test_run_args': test_run_args,
        'enable_mpi': 'yes' if projconf['run']['enable_mpi'] else 'no',
        'mpirun_args': projconf['run']['mpirun_args'],
        'compilers': ' '.join(given_compilers),
        }
    replacements.update({key + '_compiler': val
                         for key, val in base_compilers.items()})

    flitutil.process_in_file(os.path.join(conf.data_dir, 'Makefile.in'),
                             makefile, replacements, overwrite=True)

    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
