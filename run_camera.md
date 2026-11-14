# Running the Raspberry Pi Camera Program

This file contains the commands needed to run `main.py` from Windows Command Prompt or PowerShell.

## Command Prompt

Make sure VcXsrv/XLaunch is running first.

Set the display:

```cmd
set DISPLAY=localhost:0.0
```

Connect to the Raspberry Pi:

```cmd
ssh -C -Y murphymatt2005@172.20.10.9
```

After logging into the Raspberry Pi:

```bash
cd Final_Year_Project
source .venv/bin/activate
cd Rasberry_pi
python main.py
```

Check X11 forwarding if needed:

```bash
echo $DISPLAY
```

A working connection should return something similar to:

```text
localhost:10.0
```

---

## PowerShell

Make sure VcXsrv/XLaunch is running first.

Set the display:

```powershell
$env:DISPLAY="localhost:0.0"
```

Connect to the Raspberry Pi:

```powershell
ssh -C -Y murphymatt2005@172.20.10.9
```

After logging into the Raspberry Pi:

```bash
cd ~/Final_Year_Project
source .venv/bin/activate
cd Rasberry_pi
python main.py
```

Check X11 forwarding if needed:

```bash
echo $DISPLAY
```

A working connection should return something similar to:

```text
localhost:10.0
```

## Stopping the Program

Press:

```text
q
```

inside the OpenCV camera window to stop the program.