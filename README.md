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
  ![1_20230705141434](https://github.com/gaolihu/mastiff/assets/15341193/1e75bbe4-21cb-4241-a55b-c9d59cc16bb5)

#### Compile a module
- 1, execute the build script: mstf_build.sh \
usr@localhost: # ./mstf_build.sh 
- 2, choose a compiler \
  keyboard input '4'\
  4*(gcc9.4) \
  ![2_20230705141602](https://github.com/gaolihu/mastiff/assets/15341193/21c3095d-3a8e-4b55-835f-7b5e577398e7)

- 3, choose a modules to build \
  keyboard input '3'\
  3(//CyberRT_tutorials/examples/timer_component_example) \
  ![3_20230705141718](https://github.com/gaolihu/mastiff/assets/15341193/6a4cc486-e856-4fce-a2a7-521ffc99fe53)

- 4, test the compiled module \
 the target is:\
bazel-bin/CyberRT_tutorials/examples/timer_component_example
- 8, clean the project \
 usr@localhost: # mstf_clean.sh
#
