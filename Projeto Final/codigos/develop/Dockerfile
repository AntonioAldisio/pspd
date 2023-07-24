FROM ubuntu:latest

RUN apt-get update -y
RUN apt-get install -y python3 python3-pip libopenmpi-dev wget unzip openjdk-8-jdk

ENV JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64
ENV PATH=$PATH:$JAVA_HOME/bin
ENV SPARK_VERSION=3.3.2
ENV HADOOP_VERSION=2
ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1


RUN wget https://downloads.apache.org/spark/spark-$SPARK_VERSION/spark-$SPARK_VERSION-bin-hadoop$HADOOP_VERSION.tgz && \
    tar -xzf spark-$SPARK_VERSION-bin-hadoop$HADOOP_VERSION.tgz -C /opt && \
    rm spark-$SPARK_VERSION-bin-hadoop$HADOOP_VERSION.tgz

ENV SPARK_HOME=/opt/spark-$SPARK_VERSION-bin-hadoop$HADOOP_VERSION
ENV PATH=$PATH:$SPARK_HOME/bin

RUN pip3 install pyspark confluent_kafka elasticsearch

RUN apt-get install -y gcc g++ mpich libmpich-dev && mkdir -p /usr/bin/
COPY ./start.sh /usr/bin/start.sh

WORKDIR /app

COPY . .

RUN mpicc -fopenmp -o vida jogodavida_mpi_omp.c

RUN chmod +x /usr/bin/start.sh

ENTRYPOINT [ "start.sh" ]