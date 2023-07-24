FROM python:3.10.12-slim-bullseye

RUN apt-get update

WORKDIR /app

COPY ./start.sh /usr/bin/start.sh

RUN pip install elasticsearch confluent_kafka pyspark && mkdir -p /usr/bin/

COPY . .

RUN chmod +x /usr/bin/start.sh

ENTRYPOINT [ "start.sh" ]
