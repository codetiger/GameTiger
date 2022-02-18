import sys
if sys.implementation.name == "micropython":
    from machine import Pin, ADC

class Battery():
    FULL_BATTERY = 4.2
    EMPTY_BATTERY = 2.54

    def __init__(self):
        if sys.implementation.name == "micropython":
            self.vsys = ADC(29)
            self.charging = Pin(24, Pin.IN)

    def level(self):
        if sys.implementation.name != "micropython":
            return 75
        conversion_factor = 3 * 3.7 / 65535
        voltage = self.vsys.read_u16() * conversion_factor
        percentage = 100 * ((voltage - self.EMPTY_BATTERY) / (self.FULL_BATTERY - self.EMPTY_BATTERY))
        return max(min(100, percentage), 0)
    
    def isCharging(self):
        if sys.implementation.name != "micropython":
            return False
        return self.charging.value() == 1
