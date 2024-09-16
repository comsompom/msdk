from constants import EXCLUDE_JSON_PINS
import re


class JSONExcludes:
    def __init__(self, peripheral, name):
        self.peripheral = peripheral
        self.name = name

    def check_same(self):
        match_periph = re.search(EXCLUDE_JSON_PINS[0]['peripheral_pref'], self.peripheral)
        match_name = re.search(EXCLUDE_JSON_PINS[0]['name_pref'], self.name)
        if match_periph and match_name:
            return True
        return False
