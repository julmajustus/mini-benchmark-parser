## mini-benchmarker.sh
A slightly modified version of [Tor Vic's original benchmarker](https://gitlab.com/torvic9/mini-benchmarker) script that does not prompt the user to clear page caches or provide unique names for benchmark logs. This allows multiple benchmark runs to be executed without user interaction between runs.

## run-benchmarker.sh
Runs `mini-benchmarker.sh` a specified number of times.
 ### Usage
  - run-benchmarker.sh N
  - If no argument is given, `run-benchmarker.sh` defaults to 3 runs.

## install.sh
Installs `mini-benchmarker.sh` and `run-benchmarker.sh` to the user's .local/bin directory.
