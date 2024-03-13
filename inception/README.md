19/Feb/2024
https://github.com/codesshaman/inception/blob/main/00_INSTALL_SYSTEM.md

https://github.com/codesshaman/inception/blob/main/01_INSTALL_SOFT.md

https://github.com/codesshaman/inception/blob/main/02_PORTS_FORWARDING.md
<Error>
Spent 3 hours to figure out how to Login via host OS terminal
Port number should be greater than 1024...

https://github.com/codesshaman/inception/blob/main/03_CLOUD_STORAGE.md

https://github.com/codesshaman/inception/blob/main/04_FIRST_SETTINGS.md
<Error>
sudo is not working as a hongbaki user.
So i added below in root
'sudo usermod -aG sudo hongbaki'



20/Feb/2024
https://github.com/codesshaman/inception/blob/main/05_INSTALL_CERTIFICATE.md
<Error>
'sudo apt update -y'

The date command reports a wrong date and time. 
I used the following and fixed

'sudo apt install ntp' 
'sudo service ntp restart' 

<Error>
'curl -s https://api.github.com/repos/FiloSottile/mkcert/releases/latest| grep browser_download_url  | grep linux-amd64 | cut -d '"' -f 4 | wget -qi -'

There is not enough space to install.
no error msg made me to figure out for one hour...


https://github.com/codesshaman/inception/blob/main/06_MAKEFILE_CREATION.md

<Error>
fclean:
    @printf "Total clean of all configurations docker\n"
    @CONTAINERS=$$(docker ps -q); \
    if [ -n "$$CONTAINERS" ]; then \
        docker stop $$CONTAINERS; \
    fi
    @docker system prune --all --force --volumes
    @docker network prune --force
    @docker volume prune --force


https://github.com/codesshaman/inception/blob/main/07_DOCKER_NGINX.md

https://github.com/codesshaman/inception/blob/main/08_DOCKER_MARIADB.md


<Error>
hongbaki@hongbaki:~/project/srcs$ docker-compose up -d mariadb
WARNING: The DB_NAME variable is not set. Defaulting to a blank string.
WARNING: The DB_USER variable is not set. Defaulting to a blank string.
WARNING: The DB_PASS variable is not set. Defaulting to a blank string.
WARNING: The DB_ROOT variable is not set. Defaulting to a blank string.
ERROR: Named volume "db-volume:/var/lib/mysql:rw" is used in service "mariadb" but no declaration was found in the volumes section.

changed docker-compose.yml

volumes:
      - ./db-volume:/var/lib/mysql
      
      


docker run --name mariadb -e MARIADB_ROOT_PASSWORD=123654 -d mariadb

      


