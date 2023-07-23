from confluent_kafka import Consumer, KafkaError
from datetime import datetime
from elasticsearch import Elasticsearch
import os


es = Elasticsearch(['http://elastic:1h9V2jFKlgr2S6oi5R00Pf54@10.245.217.227:9200'])
index_name = 'mpi'

index_mapping = {
    'mappings': {
        'properties': {
            'content': {
                'type': 'text'
            }
        }
    }
}

# Create the index
try:
    es.indices.create(index=index_name, ignore=400, body={index_mapping})
    print("Index creation successful.")
except ConnectionError as ce:
    print("Connection error occurred:", ce)
except Exception as e:
    print("An error occurred:", e)

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

def consume_kafka_topic(topic):
  consumer = Consumer({
    'bootstrap.servers': 'kafka-service:9092',
    'group.id': 'my_consumer_group',
    'auto.offset.reset': 'earliest'
  })

  try:
    consumer.subscribe([topic])
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
  except Exception as e:
    print(f"An error occurred in the Kafka consumer: {e}")
  finally:
    consumer.close()

def read_file_and_send_to_es(codeSelector):
  try:
    with open(f'./output{codeSelector}.txt', 'r') as file:
      file_content = file.read()

      document = {'content': file_content}

      index_name = 'mpi'
      es.index(index=index_name, body=document)
      print("Enviado com sucesso.")
  except Exception as e:
      print(f"Erro: {e}")


if __name__ == "__main__":
  POWMIN_TOPIC = "pspd"
  # BROKERS = '10.244.0.149:9092'

  consume_kafka_topic(POWMIN_TOPIC)
