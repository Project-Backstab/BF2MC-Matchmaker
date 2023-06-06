# Service

If you want to install this project as a service we have here instructions how to setup.

You can check systemd version like this:
```
systemd --version
```

We reload systemd
```
sudo systemctl daemon-reload
```

Copy the service file and modify the following values in bf2mc.service: "ExecStart" and "WorkingDirectory" 
```
sudo cp service/bf2mc.service /etc/systemd/system/
sudo nano /etc/systemd/system/bf2mc.service
```

Enable and Start service:
```
sudo systemctl enable bf2mc.service
sudo systemctl start bf2mc.service
```

## Usage

```
sudo systemctl start bf2mc
sudo systemctl stop bf2mc
sudo systemctl restart bf2mc
sudo systemctl status bf2mc
```
