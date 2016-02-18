/*
	Copyright (c) 2016 Xavier Leclercq

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
	IN THE SOFTWARE.
*/

/*
	Part of this file were copied from the Chart.js project (http://chartjs.org/)
	and translated into C++.

	The files of the Chart.js project have the following copyright and license.

	Copyright (c) 2013-2016 Nick Downie
	Released under the MIT license
	https://github.com/nnnick/Chart.js/blob/master/LICENSE.md
*/

#include "wxchartgrid.h"
#include "wxchartutilities.h"
#include <wx/pen.h>
#include <sstream>

wxChartGrid::wxChartGrid(const wxPoint2DDouble &position,
						 const wxSize &size,
						 const wxVector<wxString> &labels,
						 wxDouble minValue,
						 wxDouble maxValue,
						 const wxChartGridOptions& options)
	: m_options(options), m_position(position),
	m_XAxis(new wxChartAxis(labels, options.GetXAxisOptions())),
	m_YAxis(new wxChartAxis(options.GetYAxisOptions())),
	m_mapping(size, m_XAxis, m_YAxis),
	m_needsFit(true)
{
	wxDouble effectiveMinValue = minValue;
	if (m_YAxis->GetOptions().GetStartValueMode() == wxCHARTAXISVALUEMODE_EXPLICIT)
	{
		effectiveMinValue = m_YAxis->GetOptions().GetStartValue();
	}
	wxDouble effectiveMaxValue = maxValue;
	if (m_YAxis->GetOptions().GetEndValueMode() == wxCHARTAXISVALUEMODE_EXPLICIT)
	{
		effectiveMaxValue = m_YAxis->GetOptions().GetEndValue();
	}

	wxDouble graphMinValue;
	wxDouble graphMaxValue;
	wxDouble valueRange = 0;
	wxChartUtilities::CalculateGridRange(effectiveMinValue, effectiveMaxValue,
		graphMinValue, graphMaxValue, valueRange, m_steps, m_stepValue);
	m_mapping.SetMinValue(graphMinValue);
	m_mapping.SetMaxValue(graphMaxValue);

	wxVector<wxChartLabel> yLabels;
	BuildYLabels(m_mapping.GetMinValue(), m_steps, m_stepValue, yLabels);
	m_YAxis->SetLabels(yLabels);
}

bool wxChartGrid::HitTest(const wxPoint &point) const
{
	return false;
}

wxPoint2DDouble wxChartGrid::GetTooltipPosition() const
{
	return wxPoint2DDouble(0, 0);
}

void wxChartGrid::Draw(wxGraphicsContext &gc)
{
	Fit(gc);	

	if (m_options.ShowHorizontalLines())
	{
		const wxChartAxis* verticalAxis = 0;
		if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
		{
			verticalAxis = m_XAxis.get();
		}
		else if (m_YAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
		{
			verticalAxis = m_YAxis.get();
		}

		for (size_t i = 1; i < verticalAxis->GetNumberOfTickMarks(); ++i)
		{
			wxPoint2DDouble lineStartPosition = verticalAxis->GetTickMarkPosition(i);

			wxGraphicsPath path = gc.CreatePath();
			path.MoveToPoint(lineStartPosition);
			path.AddLineToPoint(m_mapping.GetSize().GetWidth() - m_mapping.GetRightPadding() + verticalAxis->GetOptions().GetOverhang(), lineStartPosition.m_y);
			
			wxPen pen1(m_options.GetGridLineColor(), m_options.GetGridLineWidth());
			gc.SetPen(pen1);
			gc.StrokePath(path);
		}
	}

	if (m_options.ShowVerticalLines())
	{
		const wxChartAxis* horizontalAxis = 0;
		if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
		{
			horizontalAxis = m_XAxis.get();
		}
		else if (m_YAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
		{
			horizontalAxis = m_YAxis.get();
		}

		for (size_t i = 1; i < horizontalAxis->GetNumberOfTickMarks(); ++i)
		{
			wxPoint2DDouble lineStartPosition = horizontalAxis->GetTickMarkPosition(i);
			wxPoint2DDouble lineEndPosition = m_mapping.GetWindowPosition(i, m_mapping.GetMaxValue());

			wxGraphicsPath path = gc.CreatePath();
			path.MoveToPoint(lineStartPosition);
			path.AddLineToPoint(lineEndPosition.m_x, lineEndPosition.m_y - horizontalAxis->GetOptions().GetOverhang());
			
			wxPen pen1(m_options.GetGridLineColor(), m_options.GetGridLineWidth());
			gc.SetPen(pen1);
			gc.StrokePath(path);
		}
	}

	m_XAxis->Draw(gc);
	m_YAxis->Draw(gc);
}

void wxChartGrid::Resize(const wxSize &size)
{
	m_mapping.SetSize(size);
	m_needsFit = true;
}

const wxChartGridMapping& wxChartGrid::GetMapping() const
{
	return m_mapping;
}

void wxChartGrid::Fit(wxGraphicsContext &gc)
{
	if (!m_needsFit)
	{
		return;
	}

	wxDouble startPoint = m_mapping.GetSize().GetHeight() - (m_YAxis->GetOptions().GetFontOptions().GetSize() + 15) - 5; // -5 to pad labels
	wxDouble endPoint = m_YAxis->GetOptions().GetFontOptions().GetSize();

	// Apply padding settings to the start and end point.
	//this.startPoint += this.padding;
	//this.endPoint -= this.padding;

	m_YAxis->UpdateLabelSizes(gc);
	m_XAxis->UpdateLabelSizes(gc);

	wxDouble leftPadding = 0;
	wxDouble rightPadding = 0;
	CalculatePadding(*m_XAxis, *m_YAxis, leftPadding, rightPadding);
	
	if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
	{
		m_XAxis->Fit(wxPoint2DDouble(leftPadding, startPoint), wxPoint2DDouble(m_mapping.GetSize().GetWidth() - rightPadding, startPoint));
		m_YAxis->Fit(wxPoint2DDouble(leftPadding, startPoint), wxPoint2DDouble(leftPadding, endPoint));
	}
	else if (m_XAxis->GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
	{
		m_XAxis->Fit(wxPoint2DDouble(leftPadding, startPoint), wxPoint2DDouble(leftPadding, endPoint));
		m_YAxis->Fit(wxPoint2DDouble(leftPadding, startPoint), wxPoint2DDouble(m_mapping.GetSize().GetWidth() - rightPadding, startPoint));
	}

	m_XAxis->UpdateLabelPositions();
	m_YAxis->UpdateLabelPositions();

	m_mapping.Fit(rightPadding);

	m_needsFit = false;
}

void wxChartGrid::BuildYLabels(wxDouble minValue,
							   size_t steps,
							   wxDouble stepValue,
							   wxVector<wxChartLabel> &labels)
{
	size_t stepDecimalPlaces = wxChartUtilities::GetDecimalPlaces();

	for (size_t i = 0; i <= steps; ++i)
	{
		wxDouble value = minValue + (i * stepValue);//.toFixed(stepDecimalPlaces);
		std::stringstream valueStr;
		valueStr << value;

		labels.push_back(wxChartLabel(valueStr.str()));
	}
}

void wxChartGrid::CalculatePadding(const wxChartAxis &xAxis,
								   const wxChartAxis &yAxis,
								   wxDouble &left,
								   wxDouble &right)
{
	if (xAxis.GetOptions().GetPosition() == wxCHARTAXISPOSITION_BOTTOM)
	{
		// Either the first x label or any of the y labels can be the widest
		// so they are all taken into account to compute the left padding
		left = yAxis.GetLabelMaxWidth();
		if ((xAxis.GetLabels().size() > 0) && ((xAxis.GetLabels().front().GetSize().GetWidth() / 2) > left))
		{
			left = (xAxis.GetLabels().front().GetSize().GetWidth() / 2);
		}

		right = 0;
		if (xAxis.GetLabels().size() > 0)
		{
			right = (xAxis.GetLabels().back().GetSize().GetWidth() / 2);
		}
	}
	else if (xAxis.GetOptions().GetPosition() == wxCHARTAXISPOSITION_LEFT)
	{
		// Either the first y label or any of the x labels can be the widest
		// so they are all taken into account to compute the left padding
		left = xAxis.GetLabelMaxWidth();
		if ((yAxis.GetLabels().size() > 0) && ((yAxis.GetLabels().front().GetSize().GetWidth() / 2) > left))
		{
			left = (yAxis.GetLabels().front().GetSize().GetWidth() / 2);
		}

		right = 0;
		if (yAxis.GetLabels().size() > 0)
		{
			right = (yAxis.GetLabels().back().GetSize().GetWidth() / 2);
		}
	}
}
