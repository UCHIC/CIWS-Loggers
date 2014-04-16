load data
load date

flow_1 = data(1:203,3)
flow_2 = data(980:end,3)

flow_1_mean = mean(flow_1)
flow_2_mean = mean(flow_2)





plot(flow_1,'r')
hold on 
plot(flow_2,'b')