//-----------------------------------------------------------------------------
//  (c) 2007 by National Instruments
//  Project: GenApi
//  Author:  Eric Gross
//  $Header$
//
//  License: This file is published under the license of the EMVA GenICam  Standard Group.
//  A text file describing the legal terms is included in  your installation as 'GenICam_license.pdf'.
//  If for some reason you are missing  this file please contact the EMVA or visit the website
//  (http://www.genicam.org) for a full copy.
//
//  THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP
//  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS;
//  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY,
//  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------
/*!
\file
\brief    Definition of interface IPersistScript and class CFeatureBag
\ingroup GenApi_PublicUtilities
*/

#ifndef _GENICAM_PERSISTENCE_H
#define _GENICAM_PERSISTENCE_H

#include <GenApi/Types.h>
#include <GenApi/Pointer.h>
#include <GenApi/GenApiDll.h>
#include <GenApi/SelectorSet.h>
#include <list>
#include <iostream>
#include <cstring>
#include <locale>

namespace GENAPI_NAMESPACE
{

    //! Basic interface to persist values to
    GENICAM_INTERFACE GENAPI_DECL_ABSTRACT IPersistScript
    {
        //! sets information about the node map
        virtual void SetInfo(const GENICAM_NAMESPACE::gcstring &Info) = 0;

        //! Stores a feature
        virtual void PersistFeature(IValue& item, CSelectorSet *selectorSet = NULL) = 0;
    };

    //! Bag holding streamable features of a nodetree
    class GENAPI_DECL CFeatureBag : public IPersistScript
    {
    public:
        //! sets information about the node map
        virtual void SetInfo(const GENICAM_NAMESPACE::gcstring &Info);

        //! Stores a feature
        virtual void PersistFeature(IValue& item, CSelectorSet *selectorSet = NULL);

        //! Loads the features from the bag to the node tree
        //! \param pNodeMap The node map to load into
        //! \param Verify If true, all streamable features are read back
        //! \param pErrorList If an error occurs during loading the error message is stored in the list and the loading continues
        /*! For Verify=true the list of names in the feature bag is replayed again.
            If a node is a selector it's value is set to the value from the feature bag
            If not the value is read from the camera and compared with the value from the feature bag.
        */
        bool LoadFromBag(INodeMap *pNodeMap, bool Verify = true, GENICAM_NAMESPACE::gcstring_vector *pErrorList = NULL);

        /*! \brief Stores the streamable nodes to this feature bag
            \param pNodeMap The node map to persist
            \param MaxNumPersistSkriptEntries The max number of entries in the container; -1 means unlimited
            \param pFeatureFilter List of valid feature; all feature if NULL.
            \return number of entries in the bag
        */
        int64_t StoreToBag(INodeMap *pNodeMap, const int MaxNumPersistSkriptEntries = -1, GENICAM_NAMESPACE::gcstring_vector *pFeatureFilter = NULL);

        //! compares the content of two feature bags
        bool operator==(const CFeatureBag &FeatureBag) const;

        GENICAM_NAMESPACE::gcstring ToString();

        virtual void Destroy();

        virtual const GENICAM_NAMESPACE::gcstring& GetBagName( void ) const;
        virtual void SetBagName(const GENICAM_NAMESPACE::gcstring& bagName);

        //! fills the bag from a stream
        friend std::istream& operator >>(std::istream &is, CFeatureBag &FeatureBag);

        //! puts the bag into a stream
        friend std::ostream& operator <<(std::ostream &os, const CFeatureBag &FeatureBag);


        CFeatureBag();
        CFeatureBag(const CFeatureBag&);
        CFeatureBag& operator=(const CFeatureBag&);
        virtual ~CFeatureBag();
    private:

        CSelectorState* AllocateSelector();
        void DeleteSelector(CSelectorState*& p);

        void Clear();
        void Push(const char *name, const char *value, CSelectorState *ps);

        const GENICAM_NAMESPACE::gcstring& GetInfo() const;

        struct Triplet
        {
            GENICAM_NAMESPACE::gcstring name;
            GENICAM_NAMESPACE::gcstring value;
            CSelectorState* pState;
        };

        class GENAPI_DECL const_iterator
        {
            // Ctor / Dtor
            // -------------------------------------------------------------------------
        public:
            const_iterator(Triplet *pTriplet = NULL);
            const_iterator(const const_iterator&);
            const_iterator& operator=(const const_iterator&);
            // Operators
            // -------------------------------------------------------------------------
        public:
            const Triplet &         operator *        (void)                  const;
            const Triplet *         operator ->       (void)                  const;
            const_iterator &        operator ++       (void);
            const_iterator          operator ++       (int);
            const_iterator &        operator +=       (intptr_t iInc);
            const_iterator          operator +        (intptr_t iInc)         const;
            intptr_t                operator -        (const const_iterator &iter) const;
            bool                    operator ==       (const const_iterator &iter) const;
            bool                    operator !=       (const const_iterator &iter) const;

            // Member
            // -------------------------------------------------------------------------
        protected:
            Triplet *m_pTriplet;

        };
        const_iterator GetBegin();
        const_iterator GetBegin() const;
        const_iterator GetEnd();
        const_iterator GetEnd() const;
    private:
        struct FeatureBagImpl;
        FeatureBagImpl *m_pImpl;


        bool LoadFromBagInternal(INodeMap *pNodeMap, bool Verify /* = true */, GENICAM_NAMESPACE::gcstring_vector *pErrorList = NULL);
        int64_t StoreToBagInternal(INodeMap *pNodeMap, const int MaxNumPersistSkriptEntries = -1, GENICAM_NAMESPACE::gcstring_vector *pFeatureFilter = NULL);

        friend class CFeatureBagger;
    };

    //! the magic GUID which indicates that the file or buffer is a GenApi stream created by the CFeatureBag class. Must be the first entry of a file or section.
    #define GENAPI_PERSISTENCE_MAGIC "{05D8C294-F295-4dfb-9D01-096BD04049F4}"

    //! the magic GUID which indicates that the file is a GenApi stream file created by the CFeatureBagger class. Must be the first entry of a file.
    #define GENAPI_PERSISTENCE_MAGIC_FEATUREBAGGER "{4709CB3C-7322-4460-84C3-DA11DDA09939}"

    //! Helper function ignoring lines starting with comment character '#'
    // note: this method must be inlined because it uses istream in the parameter list
    inline std::istream& EatComments(std::istream &is)
    {
        if( is.eof() )
        {
            return is;
        }

        char FirstCharacter = static_cast<char>(is.peek());
        while( FirstCharacter == '#' )
        {
            is.ignore(1024, '\n');
            FirstCharacter = static_cast<char>(is.peek());
        }
        return is;
    }

#ifndef GENAPI_DONT_USE_DEFAULT_PERSISTENCE_FILE_FORMAT
    //! Helper function cutting off space characters
    inline char* TrimSpace( char* Name, std::istream& is )
    {
        char* pend = Name + strlen( Name );
        char* pc = Name;
        for (; pc < pend && std::isspace( *pc, is.getloc() ); ++pc);
        for (--pend; pc < pend && std::isspace( *pend, is.getloc() ); --pend);
        pend[1] = '\0';

        return pc;
    }
    inline GENICAM_NAMESPACE::gcstring& TrimSpace( GENICAM_NAMESPACE::gcstring& s, std::istream& is )
    {
        size_t iend =s.length();
        size_t ibegin = 0;
        for (; ibegin < iend && std::isspace( s[ibegin], is.getloc() ); ++ibegin);
        for (--iend; ibegin < iend; --iend)
        {
            if (!std::isspace( s[iend], is.getloc() ))
                break;
        }
        if (iend < s.length())
        {
            s.erase(iend + 1);
        }
        s.erase( 0, ibegin );

        return s;
    }
    //! reads in persistent data from a stream
    // note: this method must be inlined because it uses istream in the parameter list
    // note: May not be used as inline if called against a library where it is already compiled.
    inline std::istream& operator >>(std::istream &is, CFeatureBag &FeatureBag)
    {
        if( is.eof() )
        {
            throw RUNTIME_EXCEPTION("The stream is eof");
        }

        FeatureBag.Clear();

        GENICAM_NAMESPACE::gcstring FirstLine;
        FirstLine.reserve( 128 );
        GENICAM_NAMESPACE::getline( is, FirstLine, '\n' );
        // Check the magic
        GENICAM_NAMESPACE::gcstring MagicGUID(GENAPI_PERSISTENCE_MAGIC);
        if( GENICAM_NAMESPACE::gcstring::_npos() == FirstLine.find(MagicGUID) )
        {
            MagicGUID = GENICAM_NAMESPACE::gcstring(GENAPI_PERSISTENCE_MAGIC_FEATUREBAGGER);
            if( GENICAM_NAMESPACE::gcstring::_npos() == FirstLine.find(MagicGUID) )
            {
                throw RUNTIME_EXCEPTION("The stream is not a GenApi feature stream since it is missing the magic GUID in the first line");
            }
            throw RUNTIME_EXCEPTION("The stream has been created using the CFeatureBagger class thus must be restored using the CFeatureBagger class as well");
        }

        EatComments( is );

        GENICAM_NAMESPACE::gcstring Name, Value;
        Name.reserve( 128 );
        Value.reserve( 128 );
        while (!is.eof())
        {
            GENICAM_NAMESPACE::getline( is, Name, '\t' );
            if (is.fail())  // if reading from stream failed -> stop reading!
            {
                throw RUNTIME_EXCEPTION( "The stream holds excess data" );
                break;
            }

            Name = TrimSpace( Name, is );
            if (Name.empty())
            {
                continue;
            }

            CSelectorState *pSelectorState = FeatureBag.AllocateSelector();
            if (!pSelectorState)
            {
                throw RUNTIME_EXCEPTION( "Unable to allocate SelectorState" );
            }


            if (is.peek() == '{')
            {
                GENICAM_NAMESPACE::getline( is, Value, '\t' );
                if (is.fail())  // if reading from stream failed -> stop reading!
                {
                    break;
                }
                for (size_t iassign = Value.find_last_of( '=' ); iassign != GENICAM_NAMESPACE::gcstring::_npos(); iassign = Value.find_last_of( '=' ))
                {
                    const size_t iend = Value.find_last_of( '}');
                    const size_t ibeg = (Value[0] == '{') ? 1 : 0;
                    const size_t rhs_len = iend == GENICAM_NAMESPACE::gcstring::_npos() ? iend : iend - iassign - 1;
                    const size_t lhs_len = iassign - ibeg;
                    GENICAM_NAMESPACE::gcstring selectorName = Value.substr( ibeg, lhs_len );
                    GENICAM_NAMESPACE::gcstring selectorValue = Value.substr( iassign + 1, rhs_len );
                    pSelectorState->AddSelector( selectorName, selectorValue );
                    if (iend != GENICAM_NAMESPACE::gcstring::_npos())
                    {
                        break;
                    }

                    GENICAM_NAMESPACE::getline( is, Value, '\t' );
                    if (is.fail())
                    {
                        break;
                    }
                }
            }
            GENICAM_NAMESPACE::getline( is, Value, '\n' );
            if (Value.empty())
            {
                FeatureBag.DeleteSelector(pSelectorState);
                throw RUNTIME_EXCEPTION( "The stream holds incomplete key - value - set" );
            }
            FeatureBag.Push( Name.c_str(), Value.c_str(), pSelectorState );

            EatComments( is );
        }
        return is;
    }

    //! writes out persistent data to a stream
    // note: this method must be inlined because it uses ostream in the parameter list
    // note: May not be used as inline if called against a library where it is already compiled.
    inline std::ostream& operator <<(std::ostream &os, const CFeatureBag &FeatureBag)
    {
        os << "# " GENAPI_PERSISTENCE_MAGIC "\n";
        if( !FeatureBag.GetInfo().empty() )
        {
            os << "# GenApi persistence file (version " << GENAPI_VERSION_MAJOR << "." << GENAPI_VERSION_MINOR << "." << GENAPI_VERSION_SUBMINOR << ")\n";
            os << "# " << FeatureBag.GetInfo() << "\n";
        }
        for (
            CFeatureBag::const_iterator it = FeatureBag.GetBegin(), end = FeatureBag.GetEnd();
            it != end;
            ++it
            )
        {
            const GENICAM_NAMESPACE::gcstring Name(it->name);
            const GENICAM_NAMESPACE::gcstring Value(it->value);
            if ((it->pState)->IsEmpty())
            {
                os << Name << "\t" << Value << "\n";
            }
            else
            {
                bool isFirst = true;
                os << Name << "\t{";
                (it->pState)->SetFirst();
                do
                {
                    if (!isFirst)
                        os << "\t";
                    else
                        isFirst = false;
                    os << (it->pState)->GetNodeName() << "=" << (it->pState)->GetNodeValue();
                } while ((it->pState)->SetNext());
                os << "}\t" << Value << "\n";
            }
        }

        return os;
    }
#endif // #ifndef GENAPI_DONT_USE_DEFAULT_PERSISTENCE_FILE_FORMAT

    //! Class use to bag features
    class GENAPI_DECL CFeatureBagger
    {
    public:
        class GENAPI_DECL const_iterator
        {
            // Ctor / Dtor
            // -------------------------------------------------------------------------
        public:
            const_iterator(CFeatureBag **ppBag = NULL);

            // Operators
            // -------------------------------------------------------------------------
        public:
            const CFeatureBag &     operator *        (void)                  const;
            const CFeatureBag *     operator ->       (void)                  const;
            const_iterator &        operator ++       (void);
            const_iterator          operator ++       (int);
            const_iterator &        operator +=       (intptr_t iInc);
            const_iterator          operator +        (intptr_t iInc)         const;
            intptr_t                operator -        (const const_iterator &iter) const;
            bool                    operator ==       (const const_iterator &iter) const;
            bool                    operator !=       (const const_iterator &iter) const;

            // Member
            // -------------------------------------------------------------------------
        protected:
            CFeatureBag **_ppb;
        };
        explicit CFeatureBagger();
        virtual ~CFeatureBagger();

        /*! \brief Stores the streamable nodes to this feature bags in different device configurations
        \param handleDefaultNodeMap Stores the current nodemap if True
        \param handleUserSets Store all user sets if True
        \param handleSequencerSets Store all sequencer sets if True
        \param pNodeMap The node map to persist
        \param MaxNumPersistSkriptEntries The max number of entries in the container. -1 means unlimited
        \return number of bags
        */
        size_t Bag(INodeMap *pNodeMap, bool handleDefaultNodeMap = true, bool handleUserSets = false, bool handleSequencerSets = false, const int MaxNumPersistSkriptEntries = -1);

        //! Loads the features from the bag to the node tree
        //! \param Verify If true, all streamable features are read back
        //! \param pErrorList If an error occurs during loading the error message is stored in the list and the loading continues
        /*! For Verify=true the list of names in the feature bag is replayed again.
            If a node is a selector it's value is set to the value from the feature bag
            If not the value is read from the camera and compared with the value from the feature bag.
        */
        bool UnBag(INodeMap *pNodeMap, bool Verify = true, GENICAM_NAMESPACE::gcstring_vector *pErrorList = NULL);

        //! sets information about the node map
        virtual void SetInfo(const GENICAM_NAMESPACE::gcstring &Info);

        // Element access
        // ---------------------------------------------------------------------------
        virtual const_iterator      begin(void)                 const;
        virtual const_iterator      end(void)                   const;
        virtual const CFeatureBag&  at(size_t uiIndex)          const;
        virtual size_t              size(void)                  const;

        //! puts the bags into a stream
        friend std::ostream& operator <<(std::ostream &os, const CFeatureBagger &featureBagger);

        //! reads the bags from a stream
        friend std::istream& operator >>(std::istream &is, CFeatureBagger &featureBagger);

    private:
        CFeatureBagger(const CFeatureBagger&);            // do not allow copy constructor
        CFeatureBagger& operator=(const CFeatureBagger&); // do not allow assignments
        CFeatureBag& AddBag(const GENICAM_NAMESPACE::gcstring &bagName);
        void DeleteAllBags( void );
        template<class _Ty>
        void UnBagCustomAction(INodeMap *pNodeMap, _Ty setNodePtr, const GENICAM_NAMESPACE::gcstring& setNodeValue, CCommandPtr saveNodePtr );

        void *m_pBags;
        //! String describing the node map
        GENICAM_NAMESPACE::gcstring m_Info;
    };

#ifndef GENAPI_DONT_USE_DEFAULT_PERSISTENCE_FILE_FORMAT
    //! writes out persistent data to a stream
    // note: this method must be inlined because it uses ostream in the parameter list
    // note: May not be used as inline if called against a library where it is already compiled.
    inline std::ostream& operator <<(std::ostream &os, const CFeatureBagger &featureBagger)
    {
        os << "# " GENAPI_PERSISTENCE_MAGIC_FEATUREBAGGER "\n";
        if( !featureBagger.m_Info.empty() )
        {
            os << "# GenApi CFeatureBagger persistence file (version " << GENAPI_VERSION_MAJOR << "." << GENAPI_VERSION_MINOR << "." << GENAPI_VERSION_SUBMINOR << ")\n";
            os << "# " << featureBagger.m_Info << "\n";
        }

        CFeatureBagger::const_iterator it;
        for (it = featureBagger.begin(); it != featureBagger.end(); it++)
        {
            os << "[" << (*it).GetBagName() << "]\n";
            os << (*it);
        }

        return os;
    }

    //! reads back persistent data from a stream
    // note: this method must be inlined because it uses istream in the parameter list
    // note: May not be used as inline if called against a library where it is already compiled.
    inline std::istream& operator >> (std::istream &is, CFeatureBagger &featureBagger)
    {
        if (is.eof())
        {
            throw RUNTIME_EXCEPTION("The stream is eof");
        }

        // Check the magic
        GENICAM_NAMESPACE::gcstring FirstLine;
        GENICAM_NAMESPACE::getline( is, FirstLine, '\n' );
        GENICAM_NAMESPACE::gcstring MagicGUID(GENAPI_PERSISTENCE_MAGIC_FEATUREBAGGER);
        bool boCFeatureBagFormatDetected = false;
        if( GENICAM_NAMESPACE::gcstring::_npos() == FirstLine.find(MagicGUID) )
        {
            MagicGUID = GENICAM_NAMESPACE::gcstring(GENAPI_PERSISTENCE_MAGIC);
            if( GENICAM_NAMESPACE::gcstring::_npos() == FirstLine.find(MagicGUID) )
            {
                throw RUNTIME_EXCEPTION("The stream is not a GenApi feature stream since it is missing the magic GUID in the first line");
            }
            boCFeatureBagFormatDetected = true;
        }

        std::stringstream currentBagData;
        if (boCFeatureBagFormatDetected)
        {
            currentBagData << FirstLine;
        }
        else
        {
            EatComments( is );
        }

        featureBagger.DeleteAllBags();
        // Allow to digest the 'CFeatureBag' format using the 'CFeatureBagger' class!
        CFeatureBag *pBag = boCFeatureBagFormatDetected ? &featureBagger.AddBag("All") : NULL;

        while (!is.eof())
        {
            GENICAM_NAMESPACE::gcstring line;
            GENICAM_NAMESPACE::getline(is, line);
            if (is.fail())  // if reading from stream failed -> stop reading!
            {
                break;
            }
            if (!line.empty() && (line[0] == '['))
            {
                if (!currentBagData.str().empty())
                {
                    if (pBag)
                    {
                        currentBagData >> (*pBag);
                    }
                    currentBagData.str("");
                    currentBagData.clear();
                    pBag = NULL;
                }
                // this is the beginning of a new section
                const size_t pos = line.find_first_of("]");
                const GENICAM_NAMESPACE::gcstring bagName(line.substr(1, pos - 1));
                if( !bagName.empty() )
                {
                    pBag = &featureBagger.AddBag(bagName);
                }
            }
            else
            {
                currentBagData << line << "\n";
            }
        }
        if (!currentBagData.str().empty() && pBag)
        {
            currentBagData >> (*pBag);
        }
        return is;
    }
#endif // #ifndef GENAPI_DONT_USE_DEFAULT_PERSISTENCE_FILE_FORMAT
}

#endif //_GENICAM_PERSISTENCE_H
