import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.autograd import Variable
from torchvision import datasets, transforms

from sklearn.model_selection import train_test_split
from sklearn.datasets import load_wine

batch_size = 64

wine = load_wine()
wine_data = wine.data
wine_target = wine.target

train_x, train_y, test_x, train_y = train_test_split(wine_data, wine_target,test_size=0.3, shuffle=True)


train_data = datasets.MNIST(root='./data',train=True, transform=transforms.ToTensor(), download=False)
test_data = datasets.MNIST(root='./data', train=False, transform=transforms.ToTensor(), download=False)

train_loader = torch.utils.data.DataLoader(dataset=train_data, batch_size=batch_size, shuffle=True)
test_loader = torch.utils.data.DataLoader(dataset=test_data, batch_size=batch_size, shuffle=False)

class MLP(nn.Module):
    def __init__(self):
        super(MLP, self).__init__()
        self.l1 = nn.Linear(784, 520)
        self.l2 = nn.Linear(520, 320)
        self.l3 = nn.Linear(320, 240)
        self.l4 = nn.Linear(240, 120)
        self.l5 = nn.Linear(120, 10)
        # self.d_input = d_input
        # self.n_hidden = n_hidden
        # self.d_hidden = d_hidden
        # self.hidden_layers = nn.Linear()

    def forward(self, x):
        x = x.view(-1,784)
        h1 = F.relu(self.l1(x))
        h2 = F.relu(self.l2(h1))
        h3 = F.relu(self.l3(h2))
        h4 = F.relu(self.l4(h3))
        h5 = F.relu(self.l5(h4))
        output = F.softmax(h5)


        return output


learning_rate = 1e-4
model = MLP()
criterion = nn.CrossEntropyLoss()
optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate, momentum=0.9)

def train(epoch):
    model.train()
    for batch_idx, (data, target) in enumerate(train_loader):
        data, target = Variable(data), Variable(target)
        optimizer.zero_grad()
        y_pred = model(data)
        loss = criterion(y_pred, target)
        loss.backward()
        optimizer.step()
        if batch_idx % 50 == 0:
            print('Train epoch: {} batch_indx: {}  Loss: {}'.format(epoch, batch_idx, loss))

def test():
    model.eval()
    total_loss = 0

    for data, target in test_loader:
        data, target = Variable(data), Variable(target)
        y_pred = model(data)
        test_loss = criterion(y_pred, target)
        print(test_loss)


for epoch in range(1,100):
    train(epoch)
    test()


# test_x, test_y = Variable(test_X), Variable(test_Y)
# result = torch.max(model(test_x).data, 1)[1]
# accuracy = sum(test_y.data.numpy() == result.numpy()) / len(test_y.data.numpy())
# accuracy

