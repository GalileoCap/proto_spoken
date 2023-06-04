#############################################################
# S: Utils ##################################################

import os
from pathlib import Path

def filesWithExtension(d, extension):
  return [ str(fpath) for fpath in list(Path(d).rglob(f'*{extension}')) ]

def genTaskCCompile(fpath, opath):
  return {
    'name': fpath,
    'deps': [fpath],
    'outs': [opath],

    'actions': [
      f'mkdir -p {BUILDD}',
      f'clang++ -c -o {opath} {CFLAGS} {fpath}',
    ],
  }

#############################################################
# S: Config #################################################

UDOConfig = {
  'version': (1, 4, 0)
}

SRCD = 'src'
INCLUDED = os.path.join(SRCD, 'include')
EXPD = 'example'
BUILDD = 'build'

BIN = os.path.join(BUILDD, 'spoken')

CSRC = filesWithExtension(SRCD, '.cc')
CHEADS = filesWithExtension(SRCD, '.h')
CFLAGS = ' '.join([
  '-g', '-O3',
  f'-I{INCLUDED}',
])

SSRC = os.path.join(EXPD, 'test.spoken')

#############################################################
# S: Tasks ##################################################

def TaskSpoken():
  objects = [os.path.join(BUILDD, os.path.basename(fpath) + '.o') for fpath in CSRC]

  return {
    'name': 'spoken',
    'deps': CSRC + CHEADS, 
    'outs': [BUILDD, BIN],

    'subtasks': [genTaskCCompile(*args) for args in zip(CSRC, objects)],
    'actions': [
      f'clang++ -o {BIN} {CFLAGS} {" ".join(objects)}',
    ],
  }

def TaskTest():
  return {
    'name': 'test',
    'deps': [TaskSpoken, SSRC],

    'capture': 1,
    'actions': [
      f'{BIN} < {SSRC}'
    ],
  }
