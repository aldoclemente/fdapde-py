import fdapdepy as fdapde
import numpy as np

import matplotlib.pyplot as plt
from matplotlib.tri import Triangulation
import pandas as pd

nlocs = 500
p0 = np.random.uniform(0,1,nlocs)
p1 = np.random.uniform(0,1,nlocs)

u_ex = lambda p0, p1 : np.sin(np.pi*p0) * np.sin(np.pi*p1)
u_ = u_ex(p0,p1)
u_ =  u_.reshape((u_.shape[0],1))
obs = u_ + 0.05 * np.random.randn(nlocs,1)
print(obs.shape, p0.shape, p1.shape)
data = pd.DataFrame(np.matrix([obs.reshape(-1), p0, p1]).transpose(), columns=["y", "p0", "p1"])

# domain
nodes = np.loadtxt("unit_square/points.txt", dtype=float)
triangles = np.loadtxt("unit_square/elements.txt", dtype=int)
triangles = triangles - 1 # sempre stessa storia
boundary = np.loadtxt("unit_square/boundary.txt", dtype=int)
mesh = fdapde.domain(nodes, triangles, boundary)
# FE basis
basis = fdapde.functional_basis(mesh, 1)
basis.size()
# penalty
f = fdapde.function(basis)
Lf = -fdapde.laplacian(f)  
penalty = fdapde.pde(Lf) # forcing == 0.
# srpde (locations at random locations)
model = fdapde.srpde(formula="y ~ f(p0,p1)", data=data, 
                     penalty=penalty)
gcv_ = fdapde.gcv(lambda_= np.array([1e-5, 1e-4,1e-3, 1e-2, 1e-1, 1]).reshape(6,1))
model.fit(calibration = gcv_)
 
def rmse(x,y):
    return np.sqrt( np.mean( (x-y)*(x-y) ))

print("err_l_inf = ", np.max(np.abs(model.f() - u_ex(nodes[:,0], nodes[:,1]))))
print("err_l_2 = ", rmse( model.f(), u_ex(nodes[:,0], nodes[:,1])))

fig, ax = plt.subplots(1,2)
ax[0].tripcolor(nodes[:,0], nodes[:,1], u_ex(nodes[:,0], nodes[:,1]))
ax[0].set(xlim=(0, 1), ylim=(0, 1))

ax[1].tripcolor(nodes[:,0], nodes[:,1], model.f().reshape(-1))
ax[1].set(xlim=(0, 1), ylim=(0, 1))
plt.show()

# srpde (locations at nodes)
obs = u_ex(nodes[:,0], nodes[:,1]).reshape((nodes.shape[0],1)) + 0.05 * np.random.randn(nodes.shape[0],1)
data = pd.DataFrame(np.matrix([obs.reshape(-1)]).transpose(), columns=["y"])
model = fdapde.srpde(formula="y ~ f", data=data, 
                     penalty=penalty)
gcv_ = fdapde.gcv(lambda_= np.array([1e-5, 1e-4,1e-3, 1e-2, 1e-1, 1]).reshape(6,1))
model.fit(calibration = gcv_)

print("err_l_inf = ", np.max(np.abs(model.f() - u_ex(nodes[:,0], nodes[:,1]))))
print("err_l_2 = ", rmse( model.f(), u_ex(nodes[:,0], nodes[:,1])))

fig, ax = plt.subplots(1,2)
ax[0].tripcolor(nodes[:,0], nodes[:,1], u_ex(nodes[:,0], nodes[:,1]))
ax[0].set(xlim=(0, 1), ylim=(0, 1))

ax[1].tripcolor(nodes[:,0], nodes[:,1], model.f().reshape(-1))
ax[1].set(xlim=(0, 1), ylim=(0, 1))
plt.show()
