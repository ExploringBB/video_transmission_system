// -*- C++ -*-
/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v2.5.14
 * TAO and the TAO IDL Compiler have been developed by:
 *       Center for Distributed Object Computing
 *       Washington University
 *       St. Louis, MO
 *       USA
 * and
 *       Distributed Object Computing Laboratory
 *       University of California at Irvine
 *       Irvine, CA
 *       USA
 * and
 *       Institute for Software Integrated Systems
 *       Vanderbilt University
 *       Nashville, TN
 *       USA
 *       https://www.isis.vanderbilt.edu/
 *
 * Information about TAO is available at:
 *     https://www.dre.vanderbilt.edu/~schmidt/TAO.html
 **/


// TAO_IDL - Generated from
// c:\ace_wrappers\tao\tao_idl\be\be_visitor_array\array_ci.cpp:147

TAO_BEGIN_VERSIONED_NAMESPACE_DECL


ACE_INLINE
void
TAO::Array_Traits<msg::ImgData_forany>::free (
    msg::ImgData_slice * _tao_slice)
{
  msg::ImgData_free (_tao_slice);
}

ACE_INLINE
msg::ImgData_slice *
TAO::Array_Traits<msg::ImgData_forany>::dup (
    const msg::ImgData_slice * _tao_slice)
{
  return msg::ImgData_dup (_tao_slice);
}

ACE_INLINE
void
TAO::Array_Traits<msg::ImgData_forany>::copy (
    msg::ImgData_slice * _tao_to,
    const msg::ImgData_slice * _tao_from)
{
  msg::ImgData_copy (_tao_to, _tao_from);
}

ACE_INLINE
void
TAO::Array_Traits<msg::ImgData_forany>::zero (
    msg::ImgData_slice * _tao_slice)
{
  // Zero each individual element.
  for ( ::CORBA::ULong i0 = 0; i0 < 40000; ++i0)
    {
      _tao_slice[i0] = ::CORBA::Octet ();
    }
}

ACE_INLINE
msg::ImgData_slice *
TAO::Array_Traits<msg::ImgData_forany>::alloc (void)
{
  return msg::ImgData_alloc ();
}


TAO_END_VERSIONED_NAMESPACE_DECL


