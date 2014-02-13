/* This file is part of the 'stringi' library.
 *
 * Copyright 2013 Marek Gagolewski, Bartek Tartanus
 *
 * 'stringi' is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 'stringi' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with 'stringi'. If not, see <http://www.gnu.org/licenses/>.
 */


#include "stri_stringi.h"





/**
 * Sets current (default) ICU locale
 *
 * @param loc new locale (a single character string)
 * @return nothing (\code{R_NilValue})
 *
 * @version 0.1 (Marek Gagolewski)
 */
SEXP stri_locale_set(SEXP loc)
{
   const char* qloc = stri__prepare_arg_locale(loc, "locale", false);
   UErrorCode err = U_ZERO_ERROR;
   uloc_setDefault(qloc, &err);
   if (U_FAILURE(err))
      Rf_error(MSG__LOCALE_ERROR_SET); // allowed here
   return R_NilValue;
}


/** Get list of available locales
 *
 * @return character vector
 *
 * @version 0.1 (Marek Gagolewski)
 */
SEXP stri_locale_list()
{
   R_len_t c = (R_len_t)uloc_countAvailable();
   SEXP ret;
   PROTECT(ret = Rf_allocVector(STRSXP, c));

   for (R_len_t i=0; i<c; ++i) {
      const char* name = uloc_getAvailable(i);
      SET_STRING_ELT(ret, i, Rf_mkChar(name));
   }

   UNPROTECT(1);
   return ret;
}


/** Get info on default (current) ICU locale or on given locale
 *
 *  @param loc either \code{NULL} or \code{""} for default locale,
 *     or one string with locale name
 *  @return a named R list object: each item (\code{Language},
 *  \code{Country}, \code{Variant}, \code{Name}) consists of
 *  a single string
 *
 * @version 0.1 (Marek Gagolewski)
 */
SEXP stri_locale_info(SEXP loc)
{
   const char* qloc = stri__prepare_arg_locale(loc, "locale", true);
   const R_len_t infosize = 4;
   SEXP vals;

   PROTECT(vals = Rf_allocVector(VECSXP, infosize));
   for (int i=0; i<infosize; ++i)
      SET_VECTOR_ELT(vals, i, Rf_ScalarString(NA_STRING));

   UErrorCode err = U_ZERO_ERROR;
   char buf[ULOC_FULLNAME_CAPACITY]; // this is sufficient

   uloc_getLanguage(qloc, buf, ULOC_FULLNAME_CAPACITY, &err);
   if (U_FAILURE(err)) err = U_ZERO_ERROR;
   else SET_VECTOR_ELT(vals, 0, Rf_mkString(buf));

   uloc_getCountry(qloc, buf, ULOC_FULLNAME_CAPACITY, &err);
   if (U_FAILURE(err)) err = U_ZERO_ERROR;
   else SET_VECTOR_ELT(vals, 1, Rf_mkString(buf));

   uloc_getVariant(qloc, buf, ULOC_FULLNAME_CAPACITY, &err);
   if (U_FAILURE(err)) err = U_ZERO_ERROR;
   else SET_VECTOR_ELT(vals, 2, Rf_mkString(buf));

   uloc_canonicalize(qloc, buf, ULOC_FULLNAME_CAPACITY, &err);
   if (U_FAILURE(err)) err = U_ZERO_ERROR;
   else SET_VECTOR_ELT(vals, 3, Rf_mkString(buf));

   stri__set_names(vals, 4, "Language", "Country", "Variant", "Name");
   UNPROTECT(1);
   return vals;
}