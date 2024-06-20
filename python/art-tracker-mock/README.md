# Getting started
Setup virtualenv:
```sh
python3 -m venv .venv
. .venv/bin/activate
```
Install dependencies
```sh
pip install -r requirements.txt
```

## 1. Capture network trace with an actual ART tracker
## 2. Run ART Tracker mock
```sh
python3 main.py trace.pcapng 192.168.0.10 65000 127.0.0.1
```
Where `192.168.0.10` is the original destination host and `65000` the destination port.
The new destination host is `127.0.0.1` in this example. The destination port is unchanged.

It keeps sending the packages from the trace file in an endless loop until interrupted by a KeyboardInterrupt.
