from confluent_kafka import Consumer, KafkaError
from datetime import datetime
from elasticsearch import Elasticsearch
import subprocess
import os 

# Fechando conexao com Elasticsearch
es = Elasticsearch([{'host': '10.245.217.227', 'port': 9200}])


def call_c_program(mensagem):
  powmin, powmax = mensagem.split()
  powmin = powmin.decode('utf-8')
  powmax = powmax.decode('utf-8')

  os.system(f"OMP_NUM_THREADS=4 mpirun -np  4 ./teste {powmin} {powmax}")

def consume_kafka_topic(topic, brokers):
  consumer = Consumer({
    'bootstrap.servers': brokers,
    'group.id': 'my_consumer_group',
    'auto.offset.reset': 'earliest'
  })

  consumer.subscribe([topic])

  try:
    while True:
      msg = consumer.poll(1.0)

      if msg is None:
        continue
      if msg.error():
        if msg.error().code() == KafkaError._PARTITION_EOF:
          continue
        else:
          print(f"Erro: {msg.error()}")
          break
      
      print(f"Valor recebido do kafka: {msg.value()}")

      call_c_program(msg.value())
  except KeyboardInterrupt:
    pass
  finally:
    consumer.close()
  read_file_and_send_to_es()

def read_file_and_send_to_es():
  try:
    with open('./output.txt', 'r') as file:
      file_content = file.read()

      document = { 'content': file_content }

      es.index(index=python-elasticsearch, body=document)
      print("Enviado com sucesso.")
  except Exception as e:
      print(f"Erro: {e}")


if __name__ == "__main__":
  POWMIN_TOPIC = "powminTopic"
  BROKERS = "localhost:9092"

  consume_kafka_topic(POWMIN_TOPIC, BROKERS)
