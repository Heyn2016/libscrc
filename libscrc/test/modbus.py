# -*- coding:utf-8 -*-
""" Test library for CRC16 Modbus """
# !/usr/bin/python
# Python:   3.5.2+
# Platform: Windows/Linux/MacOS/ARMv7
# Author:   Heyn (heyunhuan@gmail.com)
# Program:  Test library CRC16 Module.
# Package:  pip install libscrc.
# History:  2017-08-17 Wheel Ver:0.0.3 [Heyn] Initialize
#           2020-04-30 Wheel Ver:1.3   [Heyn] Optimized code, removed two steps compute function

import unittest

import libscrc
from libscrc import _crc16

class TestCRC16Modbus( unittest.TestCase ):
    """ Test CRC16 Modbus variant.
    """

    def do_basics( self, module ):
        """ Test basic functionality.
        """
        # very basic example
        self.assertEqual( module.modbus(b'123456789'), 0x4B37 )
        self.assertNotEqual( module.modbus(b'123456'), 0xE432 )

        # more basic checks
        self.assertEqual( module.modbus( b'AAAAAAAAAAAAAAAAAAAAAA'), 0xDAB3 )

        # bigger chunks
        self.assertEqual( module.modbus( b'A' * 4096), 0x4444 )
        self.assertEqual( module.modbus( b'A' * 4096 * 10), 0x9A78 )

        # test when there are no data
        self.assertEqual( module.modbus( b'' ), 0xFFFF )


    def test_basics(self):
        """ Test basic functionality.
        """
        self.do_basics( libscrc )


    def test_basics_c(self):
        """Test basic functionality of the extension module.
        """
        self.do_basics( _crc16 )

    def test_big_chunks(self):
        """Test calculation of CRC on big chunks of data.
        """
        self.assertEqual( _crc16.modbus( b'A' * 16 * 1024 * 1024 ), 0x588F )


if __name__ == '__main__':
    unittest.main()
