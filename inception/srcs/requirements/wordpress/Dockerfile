# FROM debian:bullseye

# ARG DB_NAME \
#     DB_USER \
#     DB_PASS

# # Install PHP and its extensions
# RUN apt-get update && \
#     apt-get upgrade -y && \
#     apt-get install -y php php-fpm php-mysql php-curl php-json php-mbstring php-xml php-zip wget unzip vim telnet && \
#     apt-get clean && \
#     rm -rf /var/lib/apt/lists/*

# # Create the directory for PHP-FPM socket
# RUN mkdir -p /run/php && \
#     chown www-data:www-data /run/php



# # Update www.conf file for PHP-FPM
# RUN sed -i "s|listen = /run/php/php7.4-fpm.sock|listen = 9000|g" /etc/php/7.4/fpm/pool.d/www.conf && \
#     sed -i "s|;listen.owner = nobody|listen.owner = www-data|g" /etc/php/7.4/fpm/pool.d/www.conf && \
#     sed -i "s|;listen.group = nobody|listen.group = www-data|g" /etc/php/7.4/fpm/pool.d/www.conf


# WORKDIR /var/www

# # Download and extract WordPress
# RUN wget https://wordpress.org/latest.tar.gz && \
#     tar -xzvf latest.tar.gz --strip-components=1 && \
#     rm latest.tar.gz

# # Copy wp-config-create.sh script
# #COPY ./requirements/wordpress/conf/wp-config-create.sh .
# COPY requirements/wordpress/conf/wp-config-create.sh .
# #COPY /home/hongbaki/Desktop/inc/srcs/requirements/wordpress/conf/wp-config-create.sh .



# # RUN sh wp-config-create.sh && rm wp-config-create.sh && \
# #     chmod -R 0777 wp-content/
  
# # Run wp-config-create.sh script
# RUN bash wp-config-create.sh && \
#     rm wp-config-create.sh && \
#     chmod -R 0777 wp-content/

# # Set the command to start PHP-FPM
# CMD ["/usr/sbin/php-fpm7.4", "-F"]


FROM debian:bullseye

ARG DB_NAME \
    DB_USER \
    DB_PASS

# Install PHP and its extensions
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y php7.4 php-fpm php-mysql php-curl php-json php-mbstring php-xml php-zip unzip wget vim


RUN wget https://wordpress.org/latest.zip -P /var/www


COPY ./conf/php.conf /etc/php/7.4/fpm/pool.d/www.conf

RUN cd /var/www && unzip latest.zip && rm latest.zip


RUN	chown -R root:root /var/www/wordpress && wget https://raw.githubusercontent.com/wp-cli/builds/gh-pages/phar/wp-cli.phar
RUN chmod +x wp-cli.phar
RUN mv wp-cli.phar /usr/local/bin/wp

COPY tools/auto-config.sh .

ENTRYPOINT [ "bash", "auto-config.sh" ]
