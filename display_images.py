import numpy as np
import matplotlib.pyplot as plt

img = np.fromfile('input512.raw', dtype=np.float32).reshape(512, 512).transpose()
rec = np.fromfile('output512.raw', dtype=np.float32).reshape(512, 512).transpose()
prj = np.fromfile('prj_view720_dct1024.raw', dtype=np.float32).reshape(720, 1024).transpose()
flt = np.fromfile('flt_view720_dct1024.raw', dtype=np.float32).reshape(720, 1024).transpose()

plt.subplot(221)
plt.imshow(img, cmap='gray', vmin=0, vmax=1)
plt.axis('image')
plt.title('Ground truth')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')

plt.subplot(223)
plt.imshow(rec, cmap='gray', vmin=0, vmax=1)
plt.axis('image')
plt.title('Reconstruction')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')

plt.subplot(222)
plt.imshow(prj, cmap='gray', vmin=0, vmax=150)
plt.title('Projection')
plt.xlabel('Angle')
plt.ylabel('Detector')

plt.subplot(224)
plt.imshow(flt, cmap='gray', vmin=-3, vmax=3)
plt.title('Filtration')
plt.xlabel('Angle')
plt.ylabel('Detector')

plt.show()
