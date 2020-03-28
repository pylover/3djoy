# 3djoy

Control your CNC or 3D-Printer using game controller.


### Install

```bash
make
sudo make install
```

### Quickstart

```bash
3djoy --serialdevice /dev/ttyACM0 /dev/input/js0 # Default baudrate: 115200
```

#### Help

```bash
3djoy --help
TODO: put help here
```


### Systemd

Create a file named `/etc/systemd/system/3djoy.service`.

```systemd
[Unit]
Description=Serial port multiplexer
After=network.target

[Service]
ExecStart=/usr/local/bin/3djoy --baudrate 115200 /dev/ttyAMA0
Restart=on-failure
KillSignal=SIGINT

[Install]
WantedBy=multi-user.target

```

Then:

```bash
systemctl daemon-reload
systemctl enable 3djoy.service
service 3djoy start
```

Check the service using:

```bash
ss -lnp | grep serialhub
```

