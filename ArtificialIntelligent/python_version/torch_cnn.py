import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.autograd import Variable
from torchvision import datasets, transforms

batch_size = 64
train_data = datasets.MNIST(root='./data',train=True, transform=transforms.ToTensor(), download=False)
test_data = datasets.MNIST(root='./data', train=False, transform=transforms.ToTensor(), download=False)

train_loader = torch.utils.data.DataLoader(dataset=train_data, batch_size=batch_size, shuffle=True)
test_loader = torch.utils.data.DataLoader(dataset=test_data, batch_size=batch_size, shuffle=False)

class CNN(nn.Module):
    
    def __init__(self, kernel_size):
        super(CNN, self).__init__()
        self.conv1 = nn.Conv2d(1, 10, kernel_size=kernel_size)
        self.conv2 = nn.Conv2d(10, 20, kernel_size=kernel_size)
        self.pool = nn.MaxPool2d(2,2)
        self.fc = nn.Linear(320,10)

    def forward(self, x):
        in_size = x.size(0)
        x = F.relu(self.pool(self.conv1(x)))
        x = F.relu(self.pool(self.conv2(x)))
        x = x.view(in_size,-1)
        x = self.fc(x)
        output = F.log_softmax(x)
        return output

learning_rate = 1e-4
kernel_size = 5
model = CNN(kernel_size)
# criterion = F.nll_loss()
optimizer = torch.optim.SGD(model.parameters(), lr=learning_rate, momentum=0.9)

def train(epoch):
    model.train()
    for batch_idx, (data, target) in enumerate(train_loader):
        data, target = Variable(data), Variable(target)
        optimizer.zero_grad()
        y_pred = model(data)
        # print(y_pred.size(), target.size())
        loss = F.nll_loss(y_pred, target)
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
        test_loss = F.nll_loss(y_pred, target)
        print(test_loss)


for epoch in range(1,100):
    train(epoch)
    test()
