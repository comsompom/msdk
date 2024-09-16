from dataclasses import dataclass


@dataclass
class CFileConst:
    from_pin: str = 'From_pin'
    to_pin: str = 'To_pin'
    alts: str = 'Alts'
    pad: str = 'pad'
    vdd: str = 'vdd'
    drv: str = 'drv'
    pin_func: str = 'C_pin_func'


@dataclass
class JSONStruct:
    dict_pack_name: str = 'Package'
    dict_pin_name: str = 'Pin'
    dict_peripheral_name: str = 'Peripheral'
    dict_signal: str = 'Signal'
    dict_from_pin: str = 'From_pin'
    dict_to_pin: str = 'To_pin'
    dict_description: str = 'Desc'
    dict_names: str = 'Names'
    dict_registers: str = 'Registers'
    json_np: str = 'Not Present'
    json_pack: str = 'Packages'
    json_pin: str = 'Pins'
    json_peripheral: str = 'Peripheral'
    json_pin_mux_conf: str = 'PinMuxConfig'
    json_reg: str = 'Register'
    json_reg_pref: str = 'GPIO'
    json_field: str = 'Field'
    json_signal: str = 'Signals'
    json_name: str = 'Name'
    json_pin_mux_name_zephyr: str = 'PinMuxNameZephyr'


@dataclass
class CSVHeaders:
    json_path: str = 'Pack_Pin_Periph'
    signal_name: str = 'Name'
    from_pin: str = 'From_pin'
    to_pin: str = 'To_pin'
    json_desc: str = "Description"
    function: str = 'Func_json'
    alts: str = 'ALTS'
    pad: str = 'PAD'
    vdd: str = 'VDD'
    drv: str = 'DRV'
    json_have: str = 'In_JSON'
    c_have: str = 'In_C_File'
    pin_func: str = 'C_pin_func'


@dataclass
class ChipFactor:
    tqfn: str = 'tqfn'
    wlp: str = 'wlp'
    csbga: str = 'csbga'
