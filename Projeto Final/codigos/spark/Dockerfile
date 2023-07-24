FROM python:3.10.12-slim-bullseye

RUN apt-get update

WORKDIR /app


RUN pip install elasticsearch confluent_kafka pyspark && mkdir -p /usr/bin/
COPY ./start.sh /usr/bin/start.sh

COPY . .

RUN chmod +x /usr/bin/start.sh

ENTRYPOINT [ "start.sh" ]
