/******************************************************************************
 * Copyright (c) 2020, Chen Fang <mtdcy.chen@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/


// File:    Traits.h
// Author:  mtdcy.chen
// Changes:
//          1. 20160829     initial version
//

#ifndef ABE_STL_TRAITS_H
#define ABE_STL_TRAITS_H

#include <ABE/core/Types.h>

__BEGIN_NAMESPACE_ABE

template <typename T, typename U> struct is_same    { enum { value = False }; };
template <typename T> struct is_same<T, T>          { enum { value = True  }; };

//////////////////////////////////////////////////////////////////////////////
template <typename TYPE> struct is_pointer          { enum { value = False }; };
template <typename TYPE> struct is_pointer<TYPE *>  { enum { value = True  }; };
// FIXME: different type of pointer, const TYPE * ...

template <typename TYPE> struct is_builtin          { enum { value = is_pointer<TYPE>::value }; };
template <> struct is_builtin< void     >           { enum { value = True  }; };
template <> struct is_builtin< Char     >           { enum { value = True  }; };
template <> struct is_builtin< Int8     >           { enum { value = True  }; };
template <> struct is_builtin< UInt8    >           { enum { value = True  }; };
template <> struct is_builtin< Int16    >           { enum { value = True  }; };
template <> struct is_builtin< UInt16   >           { enum { value = True  }; };
template <> struct is_builtin< Int32    >           { enum { value = True  }; };
template <> struct is_builtin< UInt32   >           { enum { value = True  }; };
template <> struct is_builtin< Int64    >           { enum { value = True  }; };
template <> struct is_builtin< UInt64   >           { enum { value = True  }; };
template <> struct is_builtin< Float32  >           { enum { value = True  }; };
template <> struct is_builtin< Float64  >           { enum { value = True  }; };

template <typename T> struct is_trivial_ctor        { enum { value = is_builtin<T>::value };    };
template <typename T> struct is_trivial_dtor        { enum { value = is_builtin<T>::value };    };
template <typename T> struct is_trivial_copy        { enum { value = is_builtin<T>::value };    };
template <typename T> struct is_trivial_move        { enum { value = is_builtin<T>::value };    };
template <typename T> struct is_trivial_move<sp<T> >{ enum { value = True };                    };
#ifdef ABE_STRING_H    // String.h
template <> struct is_trivial_move<String>          { enum { value = True };                    };
#endif
#ifdef ABE_SYSTEM_H
template <> struct is_trivial_dtor<Time>            { enum { value = is_builtin<UInt64>::value }; };
template <> struct is_trivial_copy<Time>            { enum { value = is_builtin<UInt64>::value }; };
template <> struct is_trivial_move<Time>            { enum { value = is_builtin<UInt64>::value }; };
#endif

__END_NAMESPACE_ABE

#endif // ABE_STL_TRAITS_H
