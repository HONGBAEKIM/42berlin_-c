FROM debian:bullseye

# Run package updates and install Nginx
RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install nginx openssl vim -y

# Create directory for ssl
RUN mkdir -p /etc/nginx/ssl && mkdir -p /var/run/nginx
    
# Generate SSL certificates
RUN openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
    -keyout /etc/nginx/ssl/inception.key \
    -out /etc/nginx/ssl/inception.crt \
    -subj "/C=FR/ST=Berlin/L=Berlin/O=42/OU=42/CN=hongbaki/UID=hongbaki/"

COPY ./conf/nginx.conf /etc/nginx/nginx.conf

RUN chmod 755 /var/www/html

RUN chown -R www-data:www-data /var/www/html

CMD ["nginx", "-g", "daemon off;"]

# Expose the SSL port
# EXPOSE 443
