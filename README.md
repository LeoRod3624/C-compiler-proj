As of now, I have managed to add Docker onto my project.
-----------------------------------------------------------------------------------------------------
WARNING: This project works best on linux and MAC OS. If you have a windows system it is best to use
WSL2 to not change anything and truly get the "plug and play" feeling.
-----------------------------------------------------------------------------------------------------

When cloning the repo(assuming you also know to switch to the correct branch if need be)

You must go insde the repo and run 

"docker build -t leo ."
(the tag name can be anything you want)

then run the following command to start the image and have the host files linked to the image files

"docker run --rm -it -v "$(pwd)":/app leo"
(again tag name can be different)

This will put you into the image and automatically set env var LEO_BACKEND to "llvm". Allowing you
to run all llvm tests(test_llvm.sh). If you want to change to ARM test simply run 'export LEO_BACKEND=""'.
This will then allow you to run all ARM assembly tests(test.sh)

To run either of the test files simply type their name in the home dir, which in the docker image will be set too "/app"