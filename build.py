from os import system, listdir, mkdir
from sys import platform
from json import load
from shutil import copy

def copy_sql_scripts( config ):
  for script in listdir('sql'):
    copy(
      f'sql/{script}', 
      f'{config["install-options"]["base-path"]}/Empire/Sql/{script}' 
    )

def build_install_folder( config ):
  mkdir( f'{config["install-options"]["base-path"]}/Empire' )
  mkdir( f'{config["install-options"]["base-path"]}/Empire/Shaders' )
  mkdir( f'{config["install-options"]["base-path"]}/Empire/Sql' )
  mkdir( f'{config["install-options"]["base-path"]}/Empire/Data' )
  mkdir( f'{config["install-options"]["base-path"]}/Empire/Logs' )

def build_shaders( config ):
  with open('shaders/config.json', 'r') as json_file:
    shader_infos = load(json_file)

  for info in shader_infos:
    command = 'glslc '
    
    for shader in info['source']:
      command += f'shaders/{shader} '
    
    for flag in info['flags']:
      command += flag + ' '
    
    command += f'-o{config["install-options"]["base-path"]}/Empire/Shaders/{info["name"]}'
    system( command )

def program_build_command( config ):
  command = 'clang++ '

  for source in listdir( 'source' ):
    command += f'source/{source} '
  
  command += '--std=c++20 '

  if config['install-options']['debug']:
    command += '-g3 '

  vulkan_base = config['vulkan-sdk']['base-path']
  vulkan_name = config['vulkan-sdk']['vulkan-name']

  sdl_name = 'SDL2'
  sdl_main_name = 'SDL2main'
  sdl_ttf_name = 'SDL2_ttf'

  command += f'-I{vulkan_base}/Include '
  command += f'-L{vulkan_base}/Lib '
  command += f'-l{vulkan_name} '

  command += '-I3dparty/sdl2/include '
  command += '-L3dparty/sdl2 '
  command += f'-l{sdl_name} '
  command += f'-l{sdl_main_name} '
  command += f'-l{sdl_ttf_name} '
  
  if 'win' in platform:
    extension = '.exe'
  else:
    extension = None

  command += f'-o{config["install-options"]["base-path"]}/Empire/empire{extension}'
  return command

def copy_libs(config):
  if 'win' in platform:
    extension = '.dll'
    extension_2 = '.lib'
  else:
    extension = '.so'
    extension_2 = '.a'

  copy(f'3dparty/sdl2/SDL2_ttf{extension}', f'{config["install-options"]["base-path"]}/Empire/SDL2_ttf{extension}')
  copy(f'3dparty/sdl2/SDL2_ttf{extension}', f'{config["install-options"]["base-path"]}/Empire/SDL2_ttf{extension}')
  copy(f'3dparty/sdl2/SDL2{extension}', f'{config["install-options"]["base-path"]}/Empire/SDL2{extension}')
  copy(f'3dparty/sdl2/SDL2main{extension_2}', f'{config["install-options"]["base-path"]}/Empire/SDL2main{extension_2}')

def main():
  with open('install.json', 'r') as json_file:
    print('Loading configuration file...')
    config = load(json_file)

  if not 'Empire' in listdir( config['install-options']['base-path'] ):
    print('Setting up install folders...')
    build_install_folder( config )
  
  print()
  
  print('Moving sql scripts to install folder...')
  copy_sql_scripts( config )
  print('Compiling shaders for vulkan...')
  build_shaders( config )
  print('Coping over nessisary shared libs...')
  copy_libs( config )
  print()

  print()
  print('Building compile command...')
  command = program_build_command( config )
  print('Running compile command...')
  print()
  
  system( command )

if __name__ == '__main__':
  main()