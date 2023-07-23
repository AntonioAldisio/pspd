from confluent_kafka import Consumer, KafkaError
from datetime import datetime
from elasticsearch import Elasticsearch
import subprocess
import os 

es = Elasticsearch(['http://a190106565:fernandocalil@cm1:9200'])


def call_c_program(mensagem):
  powmin, powmax, codeSelector = mensagem.split()
  powmin = powmin.decode('utf-8')
  powmax = powmax.decode('utf-8')
  codeSelector = codeSelector.decode('utf-8')

  if (codeSelector == 'mpi'):
    os.system(f"OMP_NUM_THREADS=4 mpirun -np  4 ./teste {powmin} {powmax}")
  else:
    os.system(f"python3 ../spark/jogodavida.py {powmin} {powmax}")
  read_file_and_send_to_es(codeSelector)

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

def read_file_and_send_to_es(codeSelector):
  try:
    with open(f'./output{codeSelector}.txt', 'r') as file:
      file_content = file.read()

      document = {'content': file_content}

      index_name = 'student-a190106565-saida'
      es.index(index=index_name, body=document)
      print("Enviado com sucesso.")
  except Exception as e:
      print(f"Erro: {e}")


if __name__ == "__main__":
  POWMIN_TOPIC = "powminTopic"
  BROKERS = "localhost:9092"

  consume_kafka_topic(POWMIN_TOPIC, BROKERS)
