![Mastiff_logo](https://github.com/gaolihu/mastiff/assets/15341193/61c3ade2-2cd5-4a98-bfa8-c19cec58edd5)
# MASTIFF Mastiff mastiff Base on Apollo Cyber
#
## Framework for Robot Software System
#
### Users' Guide

#### Environment prepare
- 1, system ubuntu 18.04 / 20.04
- 2, install make, gcc, g++, wget, curl, docker, python3 \
usr@localhost: # sudo apt-get update \
usr@localhost: # sudo ln -s /usr/bin/python3 /usr/bin/python \
usr@localhost: # sudo apt-get install --reinstall build-essential
- 3, go to the project main directory \
usr@localhost: # cd /path/to/mastiff
- 4, Load docker image \
usr@localhost: # docker load -i mstfsvr_nginx.tar
- 5, Run docker container \
usr@localhost: # ./docker/start.sh
- 6, set the working environmentt \
 usr@localhost: # source mstf_env.sh
- 7, choose workspace \
![1692420972757](https://github.com/gaolihu/mastiff/assets/15341193/4584f56f-368c-4d18-86fe-ee710ee79223) \
 1(bzel/WORKSPACE.module.ext) 

#### Compile a module
- 1, execute the build script: mstf_build.sh \
usr@localhost: # ./mstf_build.sh 
- 2, choose a compiler \
![image](https://github.com/gaolihu/mastiff/assets/15341193/467e7e03-4072-4444-addc-09474afc5aa6) \
  keyboard input '4' \
  4 (gcc9.4) \

- 3, choose a modules to build \
  keyboard input '3'\
  3(//CyberRT_tutorials/examples/timer_component_example) \
![1692420881151](https://github.com/gaolihu/mastiff/assets/15341193/0eadeff0-7594-4249-8c15-85f3651199e9) \

- 4, test the compiled module \
 the target is:\
bazel-bin/CyberRT_tutorials/examples/timer_component_example
- 5, clean the project \
 usr@localhost: # mstf_clean.sh
#
