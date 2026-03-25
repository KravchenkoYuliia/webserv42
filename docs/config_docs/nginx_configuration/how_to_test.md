## GET

create me_nginx.conf

example:
events {}

http {
    server {
        listen 8080;
        server_name localhost;

        location /test1 {
            return 200 /;
        }

        location /test2 {
            return 200 "hello";
        }

        location /test3 {
            return 301 /test2;
        }
    }
}

## run nginx with my configuration
docker run --rm -p 8080:8080 \
  -v $(pwd)/nginx.conf:/etc/nginx/nginx.conf:ro \
  nginx:latest

## open in browser or
curl -i http://localhost:8080/test1
