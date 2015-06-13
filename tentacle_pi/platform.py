# encoding: utf8
"""
@author     Alexander Rüedlinger <a.rueedlinger@gmail.com>
@date       13.06.2015

"""

import subprocess


class Platform(object):

    class I2CDevice(object):

        def __init__(self, bus, addr):
            self._bus = bus
            self._addr = addr

        @property
        def bus(self):
            return self._bus

        @property
        def addr(self):
            return self._addr

    class I2CBus(object):

        def __init__(self, dev_name, num, algo, name):
            self._dev_name = dev_name
            self._num = num
            self._algo = algo
            self._name = name
            self._i2c_devices = []
            self._update_devices()

        @property
        def algo(self):
            return self._algo

        @property
        def name(self):
            return self._name

        @property
        def path(self):
            return "/dev/%s" % self._dev_name

        @property
        def num(self):
            return self._num

        @property
        def dev_name(self):
            return self._dev_name

        @property
        def devices(self):
            return self._i2c_devices

        def update(self):
            self._i2c_devices = []
            self._update_devices()

        def find_device(self, addr):
            for i2c_device in self._i2c_devices:
                if i2c_device.addr == addr:
                    return i2c_device

        def _update_devices(self):
            out_str = subprocess.check_output(['i2cdetect', '-r', '-y', str(self._num)])
            data = out_str.split("\n")

            if len(data) > 1:
                lines = data[1:-1]

                for line in lines:
                    device_addrs = self._parse_addrs(line)
                    for device_addr in device_addrs:
                        i2c_device = Platform.I2CDevice(bus=self, addr=device_addr)
                        self._i2c_devices.append(i2c_device)

        def _parse_addrs(self, line):
            device_cands = line[3:].split(" ")  # remove xx:
            device_addrs = filter(lambda x: x != '' and x != '--' and x != 'UU', device_cands)  # ignore '', UU and --
            device_addrs = map(lambda x: int(x, 16), device_addrs)
            return device_addrs

    def __init__(self):
        self._i2c_buses = {}
        self._update_buses()

    @property
    def i2c_buses(self):
        return self._i2c_buses.values()

    def i2c_bus(self, num):
        return self._i2c_buses.get(num, None)

    @property
    def i2c_devices(self):
        devices = []
        for bus_num, i2c_bus in self._i2c_buses.items():
            devices += i2c_bus.devices
        return devices

    def update(self):
        self._i2c_buses = {}
        self._update_buses()

    def find_i2c_device(self, addr, bus=None, callback=None):
        found = []
        i2c_buses = []
        if bus is None:
            i2c_buses = self._i2c_buses.values()
        else:
            i2c_bus = self._i2c_buses.get(bus, None)
            if i2c_bus is not None:
                i2c_buses.append(i2c_bus)

        for i2c_bus in i2c_buses:
            i2c_device = i2c_bus.find_device(addr)
            if i2c_device is not None:
                found.append(i2c_device)

        if callback is not None:
            map(lambda d: callback(d), found)

        return found

    def _update_buses(self):
        try:
            out_str = subprocess.check_output(['i2cdetect', '-l'])
            buses_str = out_str.split("\n")[:-1]
            for bus_str in buses_str:
                dev_name, adapter_funcs, adapter_name, adapter_algo = map(str.strip, bus_str.split("\t"))
                bus_num = int(dev_name.replace('i2c-', ''))
                i2c_bus = self.I2CBus(dev_name=dev_name, num=bus_num, algo=adapter_algo, name=adapter_name)
                self._i2c_buses[bus_num] = i2c_bus
        except OSError:
            print "Cannot run i2cdetect -l"