project('tftp_common', ['cpp'],
	version: '1.0.0',
	default_options: 'cpp_std=c++17',
	license: 'Unlicense'
)

tftp_common_dep = declare_dependency(
	include_directories: include_directories('./tftp_common')
).as_system('system')

install_headers('tftp_common/packets.hpp' 'tftp_common/parser.hpp', 'tftp_common/tftp_common.hpp', subdir: 'tftp_common')
