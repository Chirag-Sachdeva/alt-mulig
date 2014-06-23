////////////////////////////////////////////////////////////////////////////////
// CFGRule.hpp
// Repr�sentationsklasse f�r ungewichtete kontextfreie Regeln
// TH, 16.6.2014
////////////////////////////////////////////////////////////////////////////////

#ifndef __CFGRULE_HPP__
#define __CFGRULE_HPP__

#include <vector>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>

/// CFGRule implementiert eine beliebige kontextfreie Regel (inkl. Tilgungsregel)
class CFGRule
{
public: // Typdefinitionen
  // Dies ist der einzige Ort, wo Symbol als std::string definiert wird.
  // Andere Klassen wie ContextFreeGrammar rekurrieren hierauf
  // Symbol w�rde sich auch sehr gut als Template-Parameter eignen
  typedef std::string         Symbol;
  typedef std::vector<Symbol> SymbolVector;

public:
  /// Defaultkonstruktor
  CFGRule() : valid(false) {}

  /// Konstruktor aus String-Repr�sentation
  CFGRule(const std::string& s)
  {
    valid = parse_rule(s);
  }

  /// Konstruktor, dem man beide Regelseiten geben kann
  CFGRule(const Symbol& left, const SymbolVector& right)
  : lhs(left), rhs(right), valid(!left.empty())
  {}

  /// Gleichheit von zwei Regeln
  bool operator==(const CFGRule& r) const
  {
    return lhs == r.lhs && rhs == r.rhs;
  }

  /// Gibt true zur�ck gdw. die Regel wohlgeformt ist
  //bool is_valid() const { return valid; }
  
  /// Konversionsoperator: so kann ein Regelobjekt in einem if erscheinen
  /// CFGRule r("NP --> NP PP"); if (r) { ... }
  operator bool() const
  {
    return valid;
  }
  
  /// r1 < r2
  bool operator<(const CFGRule& r) const
  {
    // Maxime: reduziere < f�r komplexe Objekte auf < (und ==) f�r die eingebetteten
    // Objekte
    // Lexikographischer Vergleich: vergleiche erst nach linker Regelseite,
    // dann nach rechter Regelseite.
    // std::string::compare() w�re hier noch etwas effizienter, allerdings
    // haben Zahlen als Symbole keine compare()-Funktion, daf�r aber < und ==
    if (lhs < r.lhs) return true;
    if (lhs == r.lhs) return rhs < r.rhs;
    return false;
    //* Alternative (hat den Vorteil, weniger Annahmen (Verzicht auf ==) zu machen):
    //* if (lhs < r.lhs) return true;
    //* if (r.lhs < lhs) return false;
    //* return rhs < r.rhs;
  }

  /// Indexoperator (nur lesen) auf die Symbole der rechten Regelseite
  const Symbol& operator[](unsigned pos) const
  {
    static const Symbol invalid;
    return (pos < rhs.size()) ? rhs[pos] : invalid;
  }

  /// Akzessor f�r lhs
  const Symbol& get_lhs() const { return lhs; }

  /// Akzessor f�r rhs
  const SymbolVector& get_rhs() const { return rhs; }

  /// Gibt die L�nge der rechten Regelseite zur�ck.
  unsigned arity() const
  {
    return rhs.size();
  }

  /// Ausgabeoperator f�r CFGRule
  friend std::ostream& operator<<(std::ostream& o, const CFGRule& r)
  {
    o << r.lhs << " -->";
    for (unsigned i = 0; i < r.rhs.size(); ++i) {
      o << " " << r.rhs[i];
    }
    return o;
  }

private: // Funktionen
  bool parse_rule(const std::string& s)
  {
    typedef boost::char_separator<char>     CharSeparator;
    typedef boost::tokenizer<CharSeparator> Tokenizer;
    typedef std::vector<std::string>        StringVector;
    
    Tokenizer tokens(s, CharSeparator("\t "));
    StringVector vtokens(tokens.begin(),tokens.end());
    if (vtokens.size() >= 2) {
      if (vtokens[1] != "-->") {
        std::cerr << "CFGRule: missing arrow in rule '" << s << "'\n";
        return false;
      }
      else if (vtokens[0] == "-->") {
        std::cerr << "CFGRule: missing left-hand side in rule '" << s << "'\n";
        return false;
      }
      else {
        // OK, setze lhs und rhs
        lhs = vtokens[0];
        rhs.assign(vtokens.begin()+2,vtokens.end());
      }
    }
    else {
      std::cerr << "CFGRule: Too few components in rule '" << s << "'\n"; 
      return false;
    }
    
    return true;
  }

private:
  Symbol        lhs;    ///< Linke Seite
  SymbolVector  rhs;    ///< Rechte Seite
  bool          valid;  ///< Ist die Regel wohlgeformt
}; // CFGRule

#endif
