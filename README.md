![Mastiff_logo](https://github.com/gaolihu/mastiff/assets/15341193/61c3ade2-2cd5-4a98-bfa8-c19cec58edd5)
# MASTIFF Mastiff mastiff
#
## Framework for Robot Software System
#
### Guide
- 1, go to the main directory \
usr@localhost: # cd /path/to/mastiff 
- 2, set the working environmentt \
 usr@localhost: # source mstf_env.sh
- 3, choose workspace \
 2(bzel/WORKSPACE.module.mstf)
- 4, choose a compiler \
  keyboard input '4'\
  4*(gcc9.4)
- 5, build modules \
 usr@localhost: # ./mstf_build.sh
- 6, choose a modules to build \
  keyboard input '3'\
  3(//CyberRT_tutorials/examples/timer_component_example)
- 7, test the compiled module \
 the target is:\
bazel-bin/CyberRT_tutorials/examples/timer_component_example
- 8, clean the project \
 usr@localhost: # mstf_clean.sh
#
