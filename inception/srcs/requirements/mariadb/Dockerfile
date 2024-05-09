FROM debian:bullseye

RUN apt-get update -y && apt-get upgrade -y && apt-get install mariadb-server -y

COPY conf/50-server.cnf /etc/mysql/mariadb.conf.d/50-server.cnf
COPY tools/script.sh /

ENTRYPOINT ["bash", "/script.sh"]



# FROM debian:bullseye

# ARG DB_NAME \
#     DB_USER \
#     DB_PASS

# RUN apt-get update && \
#     apt-get install -y mariadb-server vim telnet \ 
#     mariadb-client \
#     vim \
#     && rm -rf /var/lib/apt/lists/*

# # # Copy mysqld binary to /usr/bin/mysqld
# # RUN find / -type f -executable -name 'mysqld' -exec cp {} /usr/bin/mysqld \;

# # Symlink mysqld binary to /usr/bin/mysqld
# RUN ln -s /usr/sbin/mysqld /usr/bin/mysqld

# # Create necessary directories and set permissions
# RUN mkdir -p /var/run/mysqld && \
#     chmod 777 /var/run/mysqld && \
#     mkdir -p /etc/my.cnf.d

# # Create a Docker configuration file
# RUN { \
#         echo '[mysqld]'; \
#         echo 'skip-host-cache'; \
#         echo 'skip-name-resolve'; \
#         echo 'bind-address=0.0.0.0'; \
#     } > /etc/my.cnf.d/docker.cnf

# # Create MariaDB configuration directory if it doesn't exist
# RUN mkdir -p /etc/mysql/mariadb.conf.d

# # Create an empty MariaDB server configuration file if it doesn't exist
# RUN touch /etc/mysql/mariadb.conf.d/50-server.cnf

# # Modify MariaDB server configuration
# RUN sed -i "s|skip-networking|skip-networking=0|g" /etc/mysql/mariadb.conf.d/50-server.cnf

# # Run package updates and install mariadb
 


# # Expose the port MariaDB is reachable on
# EXPOSE 3306

# # Cleanup to reduce image size
# RUN rm -rf /var/lib/apt/lists/*

# RUN mysql_install_db --user=mysql --datadir=/var/lib/mysql

# #COPY conf/create_db.sh .

# COPY requirements/mariadb/conf/create_db.sh .


# RUN sh create_db.sh && rm create_db.sh
# USER mysql
# CMD ["/usr/bin/mysqld", "--skip-log-error"]
