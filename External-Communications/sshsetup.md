On local
Add the following to .ssh/config
```bash
Host SOC
    HostName stu.comp.nus.edu.sg
    User kaijiel

Host Ultra96
    HostName 192.168.95.236
    User xilinx
    ProxyJump SOC
    ForwardAgent yes
    IdentityFile ~/.ssh/id_rsa_soc
```
On terminal do:
```bash
ssh-keygen -t rsa
ssh-copy-id kaijiel@stu.comp.nus.edu.sg
```


SSH into SOC server 
```bash
ssh SOCJ
```

On server do
```bash
ssh-keygen -t rsa
ssh-copy-id xilinx@192.168.95.236
```

Back on local do:
```bash
scp SOC:.ssh/id_rsa ~/.ssh/id_rsa_soc
```

After this, you should be able to ssh directly from local to Ultra96 by doing
```bash
ssh Ultra96
```