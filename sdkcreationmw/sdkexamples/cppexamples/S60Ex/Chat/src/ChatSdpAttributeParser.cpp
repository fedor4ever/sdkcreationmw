/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


// INCLUDE FILES
#include <bt_sock.h>

#include "ChatSdpAttributeParser.h"
#include "ChatSdpAttributeParser.pan"
#include "ChatSdpAttributeNotifier.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeParser()
// Construct a TChatSdpAttributeParser.
// ----------------------------------------------------------------------------
//
TChatSdpAttributeParser::TChatSdpAttributeParser(
    RArray<TChatSdpAttributeParser::TChatSdpAttributeNode>& aNodeList,
    MChatSdpAttributeNotifier& aObserver ) : 
    iObserver( aObserver ),
    iNodeList( aNodeList ),
    iCurrentNodeIndex( 0 )
    {
    iCurrentNode = iNodeList[iCurrentNodeIndex];
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::~TChatSdpAttributeParser()
// Destructor.
// ----------------------------------------------------------------------------
//
TChatSdpAttributeParser::~TChatSdpAttributeParser()
	{
	// No implementation needed	
	}
	
// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::HasFinished()
// Check if parsing processed the whole list.
// ----------------------------------------------------------------------------
//
TBool TChatSdpAttributeParser::HasFinished() const
    {
    return ( iCurrentNode.Command() == EFinished );
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::VisitAttributeValueL()
// Process a data element.
// ----------------------------------------------------------------------------
//
void TChatSdpAttributeParser::VisitAttributeValueL( CSdpAttrValue& aValue, 
    TSdpElementType aType )
    {
    switch ( iCurrentNode.Command() )
        {
        case ECheckType:
            CheckTypeL( aType );
            break;

        case ECheckValue:
            CheckTypeL( aType );
            CheckValueL( aValue );
            break;

        case ECheckEnd:
            User::Leave( KErrTooBig ); //list element contains too many items
            break;

        case ESkip:
            break;  // no checking required

        case EReadValue:
            CheckTypeL( aType );
            ReadValueL( aValue );
            break;

        case EFinished:
            User::Leave( KErrEof ); // element is after 
            return;                     // value should have ended

        default:
            Panic( EChatSdpAttributeParserInvalidCommand );
        }

    AdvanceL();
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::StartListL()
// Process the start of a data element list.
// ----------------------------------------------------------------------------
//
void TChatSdpAttributeParser::StartListL( CSdpAttrValueList& /*aList*/ )
    {
    // no checks done here
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::EndListL()
// Process the end of a data element list.
// ----------------------------------------------------------------------------
//
void TChatSdpAttributeParser::EndListL()
    {
    // check we are at the end of a list
    if ( iCurrentNode.Command() != ECheckEnd )
        {
        User::Leave( KErrGeneral );
        }

    AdvanceL();
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::CheckTypeL()
// Check the type of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TChatSdpAttributeParser::CheckTypeL( TSdpElementType aElementType ) const
    {
    if ( iCurrentNode.Type() != aElementType )
        {
        User::Leave( KErrGeneral );
        }
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::CheckValueL()
// Check the value of the current node is the same as the specified type.
// ----------------------------------------------------------------------------
//
void TChatSdpAttributeParser::CheckValueL( CSdpAttrValue& aValue ) const
    {
    switch ( aValue.Type() )
        {
        case ETypeNil:
            Panic( EChatSdpAttributeParserNoValue );
            break;

        case ETypeUint:
            if ( aValue.Uint() != ( TUint ) iCurrentNode.Value() )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeInt:
            if ( aValue.Int() != iCurrentNode.Value() )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeBoolean:
            if ( aValue.Bool() != iCurrentNode.Value() )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeUUID:
            if ( aValue.UUID() != TUUID( iCurrentNode.Value() ) )
                {
                User::Leave( KErrArgument );
                }
            break;

        case ETypeDES:
        case ETypeDEA:
            Panic( EChatSdpAttributeParserValueIsList );
            break;

        default:
            Panic( EChatSdpAttributeParserValueTypeUnsupported );
            break;
        }
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::ReadValueL()
// Pass the data element value to the observer.
// ----------------------------------------------------------------------------
//
void TChatSdpAttributeParser::ReadValueL( CSdpAttrValue& aValue ) const
    {
    iObserver.FoundElementL( iCurrentNode.Value(), aValue );
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::AdvanceL()
// Advance to the next node.
// ----------------------------------------------------------------------------
//
void TChatSdpAttributeParser::AdvanceL()
    {
    // check not at end
    if ( iCurrentNode.Command() == EFinished )
        {
        User::Leave( KErrEof );
        }

    // move to the next item
    iCurrentNode = iNodeList[++iCurrentNodeIndex];
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeNode::TChatSdpAttributeNode()
// constructor.
// ----------------------------------------------------------------------------
//
TChatSdpAttributeParser::TChatSdpAttributeNode::TChatSdpAttributeNode()
  {
  // no implementation needed
  }
            
// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeNode::~TChatSdpAttributeNode()
// destructor.
// ----------------------------------------------------------------------------
//
TChatSdpAttributeParser::TChatSdpAttributeNode::~TChatSdpAttributeNode()
  {
    // no implementation needed
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeNode::SetCommand( TNodeCommand aCommand )
// set iCommand member variable.
// ----------------------------------------------------------------------------
//            
void TChatSdpAttributeParser::TChatSdpAttributeNode::SetCommand( TNodeCommand aCommand )
    {
    iCommand = aCommand;
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeNode::SetType( TChatSdpElementType aType )
// set iType member variable.
// ----------------------------------------------------------------------------
//          
void TChatSdpAttributeParser::TChatSdpAttributeNode::SetType( TSdpElementType aType )
    {
    iType = aType;
    }

// ----------------------------------------------------------------------------
// void TChatSdpAttributeParser::TChatSdpAttributeNode::SetValue( TInt aValue )
// set iValue member variable.
// ----------------------------------------------------------------------------
//          
void TChatSdpAttributeParser::TChatSdpAttributeNode::SetValue( TInt aValue )
    {
    iValue = aValue;
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeNode::Command()
// get iCommand member variable value.
// ----------------------------------------------------------------------------
//            
TChatSdpAttributeParser::TNodeCommand TChatSdpAttributeParser::TChatSdpAttributeNode::Command() const
    {
    return iCommand;
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeNode::Type()
// get iType member variable value.
// ----------------------------------------------------------------------------
//            
TSdpElementType TChatSdpAttributeParser::TChatSdpAttributeNode::Type() const
    {
    return iType;
    }

// ----------------------------------------------------------------------------
// TChatSdpAttributeParser::TChatSdpAttributeNode::Value()
// get iValue member variable value.
// ----------------------------------------------------------------------------
//            
TInt TChatSdpAttributeParser::TChatSdpAttributeNode::Value() const
    {
    return iValue;
    }   
            

// End of File
