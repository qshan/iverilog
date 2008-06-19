#ifndef __PGenerate_H
#define __PGenerate_H
/*
 * Copyright (c) 2006-2008 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  "LineInfo.h"
# include  "StringHeap.h"
# include  "HName.h"
# include  <list>
# include  <map>
# include  "pform_types.h"

class Design;
class NetScope;
class PExpr;
class PFunction;
class PProcess;
class PTask;
class PGate;
class PWire;

/*
 * This represents a generate scheme. The interpretation of the
 * members depends on the scheme_type.
 *
 * GS_LOOP
 *
 * GS_CASE
 *    loop_test is the expression to be compared.
 *    generates contains only GS_CASE_ITEM schemes.
 * GS_CASE_ITEM
 *    The parent points to the GS_CASE that contains this item.
 *    the loop_test is compared with the parent->loop_test expression.
 */
class PGenerate : public LineInfo {

    public:
      explicit PGenerate(unsigned id_number);
      ~PGenerate();

	// Generate schemes have an ID number, for when the scope is
	// implicit.
      const unsigned id_number;
      perm_string scope_name;

      enum scheme_t {GS_NONE, GS_LOOP, GS_CONDIT, GS_ELSE,
		     GS_CASE, GS_CASE_ITEM};
      scheme_t scheme_type;

	// generate loops have an index variable and three
	// expressions: for (index = <init>; <test>; index=<step>)
      perm_string loop_index;
      PExpr*loop_init;
      PExpr*loop_test;
      PExpr*loop_step;

      map<perm_string,PWire*>wires;
      PWire* get_wire(perm_string name) const;

      list<PGate*> gates;
      void add_gate(PGate*);

      list<PProcess*> behaviors;

	// Tasks instantiated within this scheme.
      map<perm_string,PTask*> tasks;
      map<perm_string,PFunction*>funcs;

	// Generate schemes can contain further generate schemes.
      list<PGenerate*> generate_schemes;
      PGenerate*parent;

	// This method is called by the elaboration of a module to
	// generate scopes. the container is the scope that is to
	// contain the generated scope.
      bool generate_scope(Design*des, NetScope*container);

	// Elaborate signals within any of the generated scopes that
	// were made by this generate block within the given container scope.
      bool elaborate_sig(Design*des, NetScope*container) const;
      bool elaborate(Design*des, NetScope*container) const;

      void dump(ostream&out, unsigned indent) const;

    private:
      bool generate_scope_loop_(Design*des, NetScope*container);
      bool generate_scope_condit_(Design*des, NetScope*container, bool else_flag);
      bool generate_scope_case_(Design*des, NetScope*container);

	// Elaborate_scope within a generated scope.
      void elaborate_subscope_(Design*des, NetScope*scope);

	// These are the scopes created by generate_scope.
      list<NetScope*>scope_list_;
	// internal function called on each scope generated by this scheme.
      bool elaborate_sig_(Design*des, NetScope*scope) const;
      bool elaborate_(Design*des, NetScope*scope) const;

    private: // not implemented
      PGenerate(const PGenerate&);
      PGenerate& operator= (const PGenerate&);
};

#endif
