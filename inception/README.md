install VM  
https://foufou.tistory.com/62



Guest additions setting  
Be able to use  
1.copy paste between your computer and vm  
2.resize vm  
https://en.wikibooks.org/wiki/VirtualBox/Guest_Additions/Windows

```bash
sudo mount /dev/sr0 /media/cdrom
cd /media/cdrom
sudo ./VBoxLinuxAdditions.run
```


Genaral guide  
https://github.com/vbachele/Inception?tab=readme-ov-file



Launch Docker Desktop  
https://docs.docker.com/engine/install/debian/#install-using-the-repository



Launch nginx  
http://nginx.org/en/linux_packages.html

```bash
sudo apt update && sudo apt upgrade && sudo apt install nginx
```
  
http to https  
download .key

```bash
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/ssl/private/nginx-selfsigned.key -out /etc/ssl/certs/nginx-selfsigned.crt -subj "/C=MO/L=KH/O=1337/OU=student/CN=hongbaki.1337.be"
```


Make nginx custom file and link hongbaki to sites-enabled file  
Because /etc/nginx/nginx.conf includes /etc/nginx/sites-enabled/*


```bash
sudo ln -s /etc/nginx/sites-available/hongbaki /etc/nginx/sites-enabled/
```

Before test nginx always  
```bash
sudo systemctl restart nginx
sudo nginx -t
```


Now you can see   
'Welcome to nginx!'  
when you open browser   
'http://localhost/index.nginx-debian.html'  




 







