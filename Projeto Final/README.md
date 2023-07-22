# Projeto Final

## Alunos
|Matrícula | Aluno |
| -- | -- |
| 20/2028211  |  Antônio Aldísio |
|  19/00 | Lorrany  |
|  19/00 | Fernando  |



  mkdir -p $HOME/.kube
  sudo cp -i /etc/kubernetes/admin.conf $HOME/.kube/config
  sudo chown $(id -u):$(id -g) $HOME/.kube/config

kubeadm join 10.182.0.6:6443 --token anlegx.ehd23xjfm61op889 \
        --discovery-token-ca-cert-hash sha256:f56b8a9505a7ccda1bc117c4c59a2d43a164bdc016c5271718446e4a7e6225ae 
# Esse erro 
```        
The connection to the server 10.182.0.5:6443 was refused - did you specify the right host or port?
```
# solucao 
```
sudo -i

swapoff -a

exit

strace -eopenat kubectl version
```

sudo systemctl restart kubelet