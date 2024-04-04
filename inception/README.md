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

This is my hongbaki

```bash
hongbaki@debian:/etc/nginx/sites-available$ cat hongbaki   
server {
    listen 80;
    server_name hongbaki.1337.be;

    location / {
        return 301 https://$host$request_uri;
    }
}



server {
	listen 443 ssl;
	listen [::]:443 ssl;

	server_name hongbaki.1337.be;
	#	server_name localhost;

	ssl_certificate /etc/ssl/certs/nginx-selfsigned.crt;
	ssl_certificate_key /etc/ssl/private/nginx-selfsigned.key;

	ssl_protocols TLSv1.3;

	index index.php;
	root /var/www/html;

	location ~ [^/]\\.php(/|$) {
        try_files $uri =404;
        #fastcgi_pass wordpress:9000;
        #include fastcgi_params;
        #fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
    }
}
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




 







