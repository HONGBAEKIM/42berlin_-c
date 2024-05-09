# install VM  
https://foufou.tistory.com/62


```bash
su
apt-get install vim
vim /etc/sudoers
```  

Add #User privilege specification option
```bash
hongbaki ALL=(ALL:ALL) ALL
```  

```bash
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install build-essential module-assistant
sudo m-a prepare
```  




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


\\/\\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/!


# Genaral guide  

1. https://github.com/vbachele/Inception?tab=readme-ov-file

2. https://github.com/Theo2lt/Inception

3. https://medium.com/@ssterdev/inception-42-project-part-ii-19a06962cf3b  



Docker youtube
1. https://www.youtube.com/watch?v=Ps8HDIAyPD0&list=PLuHgQVnccGMDeMJsGq2O-55Ymtx0IdKWf  

2. https://www.youtube.com/watch?v=0kQC19w0gTI  

3. https://www.youtube.com/watch?v=EK6iYRCIjYs      

    
Launch Docker Desktop  
https://docs.docker.com/engine/install/debian/#install-using-the-repository


```bash
sudo docker cp wp-php:/etc/php/8.2/fpm/pool.d/www.conf ./requirement/wordpress/.
sudo docker cp nginx:/etc/nginx/sites-available/default ./requirement/nginx/.
```

```bash

sudo lsof -i :80

sudo kill -9 945 946 947 948 949


docker image ls -a


docker images | grep '<none>' | awk '{print $3}' | xargs docker rmi


docker container ls -a

docker compose up --build

sudo docker compose up --build -d


docker stop $(docker ps -aq) && docker rm $(docker ps -aq)

sudo docker stop $(sudo docker ps -aq) && sudo docker rm $(sudo docker ps -aq)

docker build -t nginx .
docker run -d --name nginx -p 8080:80 nginx
docker run -d -p 8080:80 nginx

```







```bash
docker network ls
docker network inspect [network_name]

docker network inspect srcs_default



SELECT user, host, authentication_string FROM mysql.user;


FLUSH PRIVILEGES;

```



```bash
docker exec -it mariadb bash
mysql
SHOW DATABASES;
SHOW TABLES;
CREATE DATABASE WordPress;
USE WordPress;

SELECT DATABASE();
SELECT user, host FROM mysql.user;



CREATE TABLE users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(255) NOT NULL,
    password VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

```




Launch nginx  
http://nginx.org/en/linux_packages.html

```bash
sudo apt update -y && sudo apt upgrade -y && sudo apt install nginx -y
```
  
http to https  
download .key

```bash
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/ssl/private/nginx-selfsigned.key -out /etc/ssl/certs/nginx-selfsigned.crt -subj "/C=MO/L=KH/O=42/OU=student/CN=hongbaki.42.fr"
```


Make nginx custom file and link hongbaki to sites-enabled file  
Because /etc/nginx/nginx.conf includes /etc/nginx/sites-enabled/*



```bash
sudo ln -s /etc/nginx/sites-available/hongbaki /etc/nginx/sites-enabled/
```

This is my hongbaki

```bash
hongbaki@debian:/etc/nginx/sites-available$ cat hongbaki   
server 
{
    listen 80;
    server_name hongbaki.42.fr;

    location / 
    {
        return 301 https://$host$request_uri;
    }
}



server 
{
	listen 443 ssl;
	listen [::]:443 ssl;

	server_name hongbaki.1337.be;
	#	server_name localhost;

	ssl_certificate /etc/ssl/certs/nginx-selfsigned.crt;
	ssl_certificate_key /etc/ssl/private/nginx-selfsigned.key;

	ssl_protocols TLSv1.3;

	index index.php;
	root /var/www/html;

	location ~ [^/]\\.php(/|$) 
    {
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
      
      
Mariadb
  
```bash
sudo apt update -y && sudo apt upgrade -y && sudo apt-get install mariadb-server -y
```







 







