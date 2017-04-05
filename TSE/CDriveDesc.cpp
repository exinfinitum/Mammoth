//	CDriveDesc.cpp
//
//	CDriveDesc class
//  Copyright (c) 2016 by Kronosaur Productions, LLC. All Rights Reserved.

#include "PreComp.h"

#define INERTIALESS_DRIVE_ATTRIB	CONSTLIT("inertialessDrive")
#define MAX_SPEED_ATTRIB			CONSTLIT("maxSpeed")
#define MAX_SPEED_INC_ATTRIB		CONSTLIT("maxSpeedInc")
#define POWER_USE_ATTRIB			CONSTLIT("powerUse")
#define POWER_USED_ATTRIB			CONSTLIT("powerUsed")
#define THRUST_ATTRIB				CONSTLIT("thrust")

CDriveDesc::CDriveDesc (void) :
        m_dwUNID(0),
		m_iMaxSpeedInc(0),
		m_iMaxSpeedLimit(-1),
        m_iThrust(0),
        m_iPowerUse(0),
        m_rMaxSpeed(0.0),
        m_fInertialess(false)

//  CDriveDesc constructor

    {

    }

void CDriveDesc::Add (const CDriveDesc &Src)

//  Add
//
//  Adds the properties of the drive to this one.

    {
    //  If the source has an UNID, then it means that it is a drive upgradew, so
    //  we take the UNID.

    if (Src.m_dwUNID)
		m_dwUNID = Src.m_dwUNID;

    //  Compute max speed based on our adjustments. If the drive enhancement
	//	is relative, then increase/decrease max speed.

	if (Src.m_iMaxSpeedInc)
		m_rMaxSpeed = Max(0.0, m_rMaxSpeed + (Src.m_iMaxSpeedInc * LIGHT_SPEED / 100.0));

	//	Otherwise, we bring max speed up to the upgrade's speed.

	else
		m_rMaxSpeed = Max(m_rMaxSpeed, Src.m_rMaxSpeed);

    //  Thrust adds

    m_iThrust += Src.m_iThrust;

    //  Power use is additive (unless inertialess)

	if (Src.m_fInertialess)
		m_iPowerUse = Src.m_iPowerUse;
	else
		m_iPowerUse += Src.m_iPowerUse;

    //  Take on inertialess

    if (Src.m_fInertialess)
        m_fInertialess = true;
    }

Metric CDriveDesc::AddMaxSpeed (Metric rChange)

//	AddMaxSpeed
//
//	Adds/subtracts to max speed.

	{
	if (rChange >= 0.0)
		m_rMaxSpeed = Min(m_rMaxSpeed + rChange, LIGHT_SPEED);
	else
		m_rMaxSpeed = Max(0.0, m_rMaxSpeed + rChange);

	return m_rMaxSpeed;
	}

Metric CDriveDesc::AdjMaxSpeed (Metric rAdj)

//  AdjMaxSpeed
//
//  Adjust speed

    {
    if (rAdj >= 0.0)
        m_rMaxSpeed = Min(m_rMaxSpeed * rAdj, LIGHT_SPEED);

    return m_rMaxSpeed;
    }

int CDriveDesc::AdjPowerUse (Metric rAdj)

//  AdjPowerUse
//
//  Adjust power use

    {
    if (rAdj >= 0.0)
        m_iPowerUse = mathRound(m_iPowerUse * rAdj);

    return m_iPowerUse;
    }

int CDriveDesc::AdjThrust (Metric rAdj)

//  AdjThrust
//
//  Adjust thrust

    {
    if (rAdj >= 0.0)
        m_iThrust = mathRound(m_iThrust * rAdj);

    return m_iThrust;
    }

ALERROR CDriveDesc::InitFromXML (SDesignLoadCtx &Ctx, CXMLElement *pDesc, DWORD dwUNID, bool bShipClass)

//  InitFromXML
//
//  Initializes from an XML descriptor

    {
    m_dwUNID = dwUNID;
	m_iMaxSpeedInc = pDesc->GetAttributeIntegerBounded(MAX_SPEED_INC_ATTRIB, -100, 100, 0);
	m_iMaxSpeedLimit = pDesc->GetAttributeIntegerBounded(MAX_SPEED_ATTRIB, 0, 100, -1);
	m_rMaxSpeed = (m_iMaxSpeedLimit > 0 ? m_iMaxSpeedLimit * LIGHT_SPEED / 100.0 : 0.0);

    //  Thrust

	m_iThrust = pDesc->GetAttributeInteger(THRUST_ATTRIB);

    //  Power use. Previous versions called it "powerUsed", so we check both
    //  attributes.

    if (!pDesc->FindAttributeInteger(POWER_USE_ATTRIB, &m_iPowerUse))
    	m_iPowerUse = pDesc->GetAttributeInteger(POWER_USED_ATTRIB);

    //  Other

	m_fInertialess = pDesc->GetAttributeBool(INERTIALESS_DRIVE_ATTRIB);

    return NOERROR;
    }

void CDriveDesc::Interpolate (const CDriveDesc &From, const CDriveDesc &To, Metric rInterpolate)

//  Interpolate
//
//  Initialize stats based on an interpolation between From and To.

    {
    //  Some values cannot be interpolated. We take one or the other.

    m_dwUNID = (rInterpolate >= 0.5 ? To.m_dwUNID : From.m_dwUNID);
    m_fInertialess = (rInterpolate >= 0.5 ? To.m_fInertialess : From.m_fInertialess);

    //  Scalar values can be interpolated

    m_iThrust = mathRound(mathInterpolate(From.m_iThrust, To.m_iThrust, rInterpolate));
    m_iPowerUse = mathRound(mathInterpolate(From.m_iPowerUse, To.m_iPowerUse, rInterpolate));
    m_rMaxSpeed = mathInterpolate(From.m_rMaxSpeed, To.m_rMaxSpeed, rInterpolate);
    }
