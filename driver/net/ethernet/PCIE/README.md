# PCIE
|罢重点|讲要点|补充|
|:-|:-|:-|
|PCIe总线的物理链路|可以由多条Lane(链路)组成，目前PCIe链路可以支持1、2、4、8、12、16和32个Lane，即×1、×2、×4、×8、×12、×16和×32宽度的PCIe链路。每一个Lane上使用的总线频率与PCIe总线使用的版本相关。||
|PCIe总线弱点|最突出的问题是传送延时|1.PCIe链路使用串行方式进行数据传送，然而在芯片内部，数据总线仍然是并行的，因此PCIe链路接口需要进行串并转换，这种串并转换将产生较大的延时;2.PCIe总线的数据报文需要经过事务层、数据链路层和物理层，这些数据报文在穿越这些层次时，也将带来延时|
|PCIe设备使用两种电源信号供电，分别是Vcc与Vaux，其额定电压为3.3V|||

