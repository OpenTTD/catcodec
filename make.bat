@rem $Id$

@rem This file is part of catcodec.
@rem catcodec is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
@rem catcodec is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
@rem See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with catcodec. If not, see <http://www.gnu.org/licenses/>.

cl /MT /Ot /W2 /RTC1 /EHsc /DWIN32 src/catcodec.cpp src/io.cpp src/sample.cpp src/rev.cpp
