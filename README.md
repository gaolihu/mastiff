![Mastiff_logo](https://github.com/gaolihu/mastiff/assets/15341193/61c3ade2-2cd5-4a98-bfa8-c19cec58edd5)
# MASTIFF Mastiff mastiff
#
## Framework for Robot Software System
#
### Users' Guide

#### Environment prepare
- 1, system ubuntu 18.04 / 20.04\
- 2, install make, gcc, g++, wget, curl, docker\
- 3, go to the project main directory \
usr@localhost: # cd /path/to/mastiff 
- 4, set the working environmentt \
 usr@localhost: # source mstf_env.sh
- 5, choose workspace \
 2(bzel/WORKSPACE.module.mstf) \
![image](https://github.com/gaolihu/mastiff/assets/15341193/8a6b11b1-47c5-4340-9860-082b1bb9fb3c)


#### Compile a module
- 1, execute the build script: mstf_build.sh \
usr@localhost: # ./mstf_build.sh 
- 2, choose a compiler \
  keyboard input '4'\
  4*(gcc9.4) \
![image](https://github.com/gaolihu/mastiff/assets/15341193/d8334265-2687-4ecf-8a65-56ef4222d8c7)
- 3, choose a modules to build \
  keyboard input '3'\
  3(//CyberRT_tutorials/examples/timer_component_example) \
![image](https://github.com/gaolihu/mastiff/assets/15341193/b333e19c-a0f2-4084-bc89-a816e0c6f99f)
- 4, test the compiled module \
 the target is:\
bazel-bin/CyberRT_tutorials/examples/timer_component_example
- 8, clean the project \
 usr@localhost: # mstf_clean.sh
#
