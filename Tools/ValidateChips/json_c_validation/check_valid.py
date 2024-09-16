import json
import copy
import csv
from collections import defaultdict
from data_structures import CFileConst, JSONStruct, CSVHeaders
from constants import JSON_PATH_PREFIX, JSON_FILE_NAME, C_CONST_FILE_NAME, C_CONST_STRING_PREFIX, \
    C_CONST_VAR_PREFIX, C_CONST_PIN_PREFIX, CSV_COMPARED_FILE_NAME, JSON_FUNC_NAMES, \
    NOT_PRESENT_VALUE, YES_VALUE, NO_VALUE


# JSON file parsing to the list. C file parsing to the dict
class JsonCValidator:
    """
        JSON and MSDK C files consistance validator;
    """

    def __init__(self):
        self.dict_const_pins = {}
        self.pin_mux_config_list = []
        self.left_compared_json_pins_list = []
        self.json_tit = JSONStruct()
        self.table_head = CSVHeaders()
        self.const_names = CFileConst()
        self.payload_to_save = []

    def _parse_peripheral_mux_config(self, peripheral_orig, peripheral_name, pin_mux_conf_dict, pack_name, pin_name):
        registers_set = sorted({reg[self.json_tit.json_reg].split("_")[1] for reg in pin_mux_conf_dict})
        pins_set = sorted(
            {pin[self.json_tit.json_field][len(self.json_tit.dict_pin_name.upper()):] for pin in pin_mux_conf_dict
             if pin[self.json_tit.json_field][:3] == self.json_tit.dict_pin_name.upper()})

        if len(pins_set) > 1:
            raise Exception(f"Out Pins in {peripheral_orig} - {peripheral_name} more then one: {len(pins_set)}")
        elif len(pins_set) < 1:
            return {}

        pin_from_set = sorted(
            {reg[self.json_tit.json_reg].split("_")[0][len(self.json_tit.json_reg_pref):] for reg in pin_mux_conf_dict})
        peripheral_dict = {self.json_tit.dict_signal: peripheral_orig, self.json_tit.dict_from_pin: pin_from_set[0],
                           self.json_tit.dict_to_pin: pins_set[0], self.json_tit.dict_description: peripheral_name,
                           self.json_tit.dict_names: {
                               self.json_tit.dict_pack_name: pack_name,
                               self.json_tit.dict_pin_name: pin_name,
                               self.json_tit.dict_peripheral_name: peripheral_name
                           }, self.json_tit.dict_registers: registers_set}
        return peripheral_dict

    def parse_json(self):
        with open(f"{JSON_PATH_PREFIX}/{JSON_FILE_NAME}", 'r') as file:
            data = json.load(file)

        packages_structure = data.get(self.json_tit.json_pack, defaultdict(
            lambda: self.json_tit.json_np))
        if packages_structure:
            for pack in packages_structure:
                pack_name = pack.get(self.json_tit.json_name, '')
                pack_pins = pack.get(self.json_tit.json_pin, defaultdict(
                    lambda: self.json_tit.json_np))
                if pack_pins:
                    for pin in pack_pins:
                        pin_name = pin.get(self.json_tit.json_name, '')
                        pin_signals = pin.get(self.json_tit.json_signal, defaultdict(
                            lambda: self.json_tit.json_np))
                        if pin_signals:
                            for peripheral in pin_signals:
                                peripheral_orig = peripheral.get(self.json_tit.json_peripheral, '')
                                peripheral_name = peripheral.get(self.json_tit.json_name, '')
                                pin_mux_conf_dict = peripheral.get(self.json_tit.json_pin_mux_conf, '')
                                if len(pin_mux_conf_dict):
                                    added_dict = self._parse_peripheral_mux_config(
                                        peripheral_orig, peripheral_name, pin_mux_conf_dict, pack_name, pin_name)
                                    if len(added_dict):
                                        self.pin_mux_config_list.append(added_dict)

    @staticmethod
    def _pin_subdict_create(sub_dict_pin_vals_list, c_pin_key_function):
        const_names = CFileConst()
        temp_list = sub_dict_pin_vals_list[1].replace('(', '').replace(')', '').split('|')
        to_pin_res_list = [pin[len(C_CONST_PIN_PREFIX) + 5:] for pin in temp_list]
        res_dict = {const_names.from_pin: sub_dict_pin_vals_list[0][len(C_CONST_PIN_PREFIX):],
                    const_names.to_pin: to_pin_res_list,
                    const_names.alts: sub_dict_pin_vals_list[2][len(C_CONST_PIN_PREFIX) + 1:],
                    const_names.pad: sub_dict_pin_vals_list[3][len(C_CONST_PIN_PREFIX) + 1:],
                    const_names.vdd: sub_dict_pin_vals_list[4][len(C_CONST_PIN_PREFIX) + 1:],
                    const_names.drv: sub_dict_pin_vals_list[5][len(C_CONST_PIN_PREFIX) + 1:],
                    const_names.pin_func: c_pin_key_function}
        return res_dict

    def _const_vars_string_parse(self, c_const_ctr):
        temp_str_list = c_const_ctr.replace(';', '').split('=')
        dict_pin_key = temp_str_list[0][len(C_CONST_VAR_PREFIX):].upper()
        dict_pin_vals_list = temp_str_list[1].replace('{', '').replace('}', '').split(',')
        c_pin_key_list = dict_pin_key.split('_')
        c_pin_key_function = ''
        if len(c_pin_key_list) > 1:
            c_pin_key_function = c_pin_key_list[1]
        if c_pin_key_list[0][-1] in JSON_FUNC_NAMES:
            c_pin_key_function = c_pin_key_list[0][-1]
        self.dict_const_pins[dict_pin_key] = self._pin_subdict_create(dict_pin_vals_list, c_pin_key_function)

    def parce_c_file(self):
        with open(f"{JSON_PATH_PREFIX}/{C_CONST_FILE_NAME}", 'r') as file:
            data_c_lines = file.readlines()

        c_line_idx = 0
        while c_line_idx < len(data_c_lines):
            const_check_str = ''
            if C_CONST_STRING_PREFIX in data_c_lines[c_line_idx]:
                while ';' not in data_c_lines[c_line_idx]:
                    const_check_str += data_c_lines[c_line_idx]
                    c_line_idx += 1
                const_check_str += data_c_lines[c_line_idx]
                const_check_str = const_check_str[len(C_CONST_STRING_PREFIX):].replace("\n", '').replace(' ', '')
                self._const_vars_string_parse(const_check_str)
            c_line_idx += 1

    def parse_files(self):
        self.parse_json()
        self.parce_c_file()

    def write_to_csv(self, payload, file_pref=''):
        with open(file_pref + CSV_COMPARED_FILE_NAME, 'w', newline='') as csv_file:
            field_names = [self.table_head.json_path, self.table_head.signal_name, self.table_head.from_pin,
                           self.table_head.to_pin, self.table_head.json_desc, self.table_head.function,
                           self.table_head.alts, self.table_head.pad, self.table_head.vdd, self.table_head.drv,
                           self.table_head.pin_func, self.table_head.json_have, self.table_head.c_have]
            csv_writer = csv.DictWriter(csv_file, fieldnames=field_names)
            csv_writer.writeheader()
            csv_writer.writerows(payload)
        return True

    def _create_payload_dict(self, json_signal={},
                             signal_name="",
                             func_name=None,
                             use_list=False,
                             use_dict=False):
        row_payload_dict = {
            self.table_head.json_desc: NOT_PRESENT_VALUE,
            self.table_head.json_path: NOT_PRESENT_VALUE,
            self.table_head.alts: NOT_PRESENT_VALUE,
            self.table_head.pad: NOT_PRESENT_VALUE,
            self.table_head.vdd: NOT_PRESENT_VALUE,
            self.table_head.drv: NOT_PRESENT_VALUE,
            self.table_head.function: NO_VALUE,
            self.table_head.json_have: NO_VALUE,
            self.table_head.c_have: NO_VALUE,
            self.table_head.pin_func: NO_VALUE
        }

        if use_list:
            json_path_str = f"{json_signal[self.json_tit.dict_names][self.json_tit.dict_pack_name]}" \
                            f"_Pin_" \
                            f"{json_signal[self.json_tit.dict_names][self.json_tit.dict_pin_name]}" \
                            f"_" \
                            f"{json_signal[self.json_tit.dict_names][self.json_tit.dict_peripheral_name]}"
            row_payload_dict[self.table_head.json_path] = json_path_str
            row_payload_dict[self.table_head.signal_name] = f"{json_signal[self.json_tit.dict_signal]}"
            row_payload_dict[self.table_head.from_pin] = f"{json_signal[self.json_tit.dict_from_pin]}"
            row_payload_dict[self.table_head.to_pin] = f"{json_signal[self.json_tit.dict_to_pin]}"
            row_payload_dict[self.table_head.json_desc] = f"{json_signal[self.json_tit.dict_description]}"
            row_payload_dict[self.table_head.function] = func_name or NO_VALUE
            row_payload_dict[self.table_head.json_have] = YES_VALUE

        if use_dict:
            row_payload_dict[self.table_head.alts] = f"{self.dict_const_pins[signal_name][self.const_names.alts]}"
            row_payload_dict[self.table_head.pad] = f"{self.dict_const_pins[signal_name][self.const_names.pad]}"
            row_payload_dict[self.table_head.vdd] = f"{self.dict_const_pins[signal_name][self.const_names.vdd]}"
            row_payload_dict[self.table_head.drv] = f"{self.dict_const_pins[signal_name][self.const_names.drv]}"
            row_payload_dict[self.table_head.pin_func] = f"{self.dict_const_pins[signal_name][self.const_names.pin_func]}"
            row_payload_dict[self.table_head.c_have] = YES_VALUE

        return row_payload_dict

    def _check_similar_validation(self, func_name):
        for idx, json_signal in enumerate(self.left_compared_json_pins_list):
            signal_name = json_signal[self.json_tit.dict_signal]
            if signal_name in self.dict_const_pins:
                json_from = json_signal[self.json_tit.dict_from_pin]
                json_to = json_signal[self.json_tit.dict_to_pin]
                c_from = self.dict_const_pins[signal_name][self.const_names.from_pin]
                c_to = self.dict_const_pins[signal_name][self.const_names.to_pin]
                if json_from == c_from and json_to in c_to:
                    kwargs = {
                        "json_signal": json_signal,
                        "signal_name": signal_name,
                        "func_name": func_name,
                        "use_list": True,
                        "use_dict": True
                    }
                    row_payload_dict = self._create_payload_dict(**kwargs)
                    self.payload_to_save.append(row_payload_dict)
                    self.left_compared_json_pins_list[idx] = 0
                    c_to.pop(c_to.index(json_to))
                    if len(c_to) == 0:
                        del self.dict_const_pins[signal_name]

    def _clear_json_list(self):
        self.left_compared_json_pins_list = [x for x in self.left_compared_json_pins_list if x != 0]

    def _check_c_dict_functions(self):
        checking_dict = copy.deepcopy(self.dict_const_pins)
        keys_to_remove = []
        for key, val in checking_dict.items():
            if val[self.const_names.pin_func] != "":
                if val[self.const_names.pin_func] in JSON_FUNC_NAMES:
                    keys_to_remove.append(key)
                    key = key[:-1]
                    self.dict_const_pins[key] = val
                    keys_to_remove.append(key)
                    self._check_similar_validation(val[self.const_names.pin_func])
                    self._clear_json_list()
                else:
                    keys_to_remove.append(key)
                    key = key.split("_")[0]
                    self.dict_const_pins[key] = val
                    keys_to_remove.append(key)
                    self._check_similar_validation(val[self.const_names.pin_func])
                    self._clear_json_list()

        for key in keys_to_remove:
            if key in self.dict_const_pins:
                self.dict_const_pins[key] = {}
                del self.dict_const_pins[key]

    def make_compare_validation(self):
        self.parse_files()

        self.left_compared_json_pins_list = copy.deepcopy(self.pin_mux_config_list)

        self._check_similar_validation(None)
        self._clear_json_list()

        self._check_c_dict_functions()
        print(f"Left unmatched from JSON: {len(self.left_compared_json_pins_list)}")
        print(f"Left unrecognized in C File: {len(self.dict_const_pins)}")

        # save not matched json file list
        for json_not_matched in self.left_compared_json_pins_list:
            kwargs = {
                "json_signal": json_not_matched,
                "signal_name": "",
                "func_name": NO_VALUE,
                "use_list": True,
                "use_dict": False
            }
            row_payload_dict = self._create_payload_dict(**kwargs)
            self.payload_to_save.append(row_payload_dict)
        # save left not matched dict from c file - self.dict_const_pins
        for key, val in self.dict_const_pins.items():
            row_payload_dict = {
                self.table_head.json_path: NOT_PRESENT_VALUE,
                self.table_head.signal_name: f"{key}",
                self.table_head.from_pin: f"{val[self.const_names.from_pin]}",
                self.table_head.to_pin: f"{val[self.const_names.to_pin]}",
                self.table_head.json_desc: NOT_PRESENT_VALUE,
                self.table_head.function: NO_VALUE,
                self.table_head.alts: f"{val[self.const_names.alts]}",
                self.table_head.pad: f"{val[self.const_names.pad]}",
                self.table_head.vdd: f"{val[self.const_names.vdd]}",
                self.table_head.drv: f"{val[self.const_names.drv]}",
                self.table_head.pin_func: f"{val[self.const_names.pin_func]}",
                self.table_head.json_have: NO_VALUE,
                self.table_head.c_have: YES_VALUE
            }
            self.payload_to_save.append(row_payload_dict)

        self.write_to_csv(self.payload_to_save)
        print("Done with CSV creation!")


files_validator = JsonCValidator()
files_validator.make_compare_validation()
