import smbus, sys, time
bus = smbus.SMBus(9)

data = [int(sys.argv[1])]
bus.write_i2c_block_data(0x17, 0x00, data)
